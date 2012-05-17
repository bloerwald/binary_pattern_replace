#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

typedef boost::optional<unsigned char> maybe_char_type;
typedef std::vector<maybe_char_type> maybe_char_vector_type;

maybe_char_vector_type parse_data (const std::string& input)
{
  std::vector<std::string> input_splitted;
  boost::split ( input_splitted
               , input
               , boost::is_any_of(" ")
               , boost::token_compress_on
               );

  maybe_char_vector_type data;

  std::transform ( input_splitted.begin()
                 , input_splitted.end()
                 , std::back_inserter (data)
                 , [&] (const std::string& s) -> boost::optional<unsigned char>
                       {
                         if (s != "??")
                         {
                           unsigned int out;
                           std::stringstream ss;
                           ss << "0x" << s;
                           ss >> std::hex >> out;
                           return static_cast<unsigned char> (out);
                         }
                         return boost::none;
                       }
                 );

  return data;
}

boost::optional<std::streampos> find_pattern ( std::fstream& file
                                             , const maybe_char_vector_type& pattern
                                             )
{
  typedef std::vector<unsigned char> buffer_type;
  buffer_type buffer (std::max (pattern.size() * 3, 1024UL));
  buffer_type::iterator matched (buffer.begin());
  buffer_type::iterator actual_end (buffer.begin());

  std::streampos pos (0);
  do
  {
    file.readsome (reinterpret_cast<char*> (&*buffer.begin()), buffer.size());
    actual_end = buffer.begin() + file.gcount();
    matched = std::search ( buffer.begin(), actual_end
                          , pattern.begin(), pattern.end()
                          , [] (const unsigned char& d, const maybe_char_type& p)
                               {
                                 return !p || d == *p;
                               }
                          );
    pos += std::distance (buffer.begin(), matched);
    if (matched == actual_end)
    {
      file.seekg (-pattern.size(), std::ios_base::cur);
      pos -= pattern.size();
    }
  }
  while (file.good() && matched == actual_end);

  return boost::make_optional (matched != actual_end, pos);
}

int main (int argc, char**argv)
{
  if (argc != 4)
  {
    std::cerr << "Usage: <filename> <pattern> <replacement>\n"
                 "  Where pattern is hex bytes or ??.\n"
                 "  Example: <filename> \"66 ?? 6f\" \"62 ?? 6f\".\n"
                 "    (replaces f?o with b?o)\n";
    exit (-1);
  }

  const maybe_char_vector_type pattern (parse_data (argv[2]));
  const maybe_char_vector_type replacement (parse_data (argv[3]));

  std::fstream file (argv[1], std::ios::in | std::ios::out | std::ios::binary);

  const boost::optional<std::streampos> match_pos (find_pattern (file, pattern));

  if (match_pos)
  {
    std::vector<char> original_data (replacement.size());
    std::vector<char>::iterator original_data_itr (original_data.begin());

    file.seekg (*match_pos);
    file.read (&*original_data.begin(), original_data.size());

    file.seekp (*match_pos);
    std::transform ( replacement.begin(), replacement.end()
                   , std::ostream_iterator<unsigned char> (file)
                   , [&] (const maybe_char_type& p) -> char
                        {
                          unsigned char res (p ? *p : *original_data_itr);
                          ++original_data_itr;
                          return *reinterpret_cast<char*> (&res);
                        }
                   );
  }

  return 0;
}
