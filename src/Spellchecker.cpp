#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <tuple>
#include <sstream>
#include <vector>

using namespace std;


struct Dictionary
{
    string NAME;
    set<string> CONTENT;
    bool VALID;
};

void toLowerCase(string& text, size_t begin = 0, size_t end = std::numeric_limits<size_t>::max())
{
    if (end > text.size()) end = text.size();

    std::for_each(text.begin(), text.begin() + end, [](char& c)
    {
        c = ::tolower(c);
    });
}

set<string> split(string s, char del = '\n')
{
    set<string> returnSet;
    string temp = "";
    for (int i = 0; i < (int)s.size(); ++i)
    {
        if (s[i] != del)
        {
            temp += s[i];
        }
        else
        {
            toLowerCase(temp);
            returnSet.insert(temp);
            temp = "";
        }
    }
    return returnSet;
}


bool read_all_words(string filename, set<string>& dest)
{
    ifstream data(filename, std::ios::in);

    if (!data.good())
    {
        cout << "Error while opening file: " << filename << endl;
        return false;
    }

    std::stringstream sstream;
    sstream << data.rdbuf();

    string text = sstream.str();

    dest = split(text);
    return true;
}

Dictionary get_dictionary_wpath(const string& path)
{
    Dictionary dict;

    size_t last = path.rfind("/", path.npos);

    dict.NAME = path.substr(last + 1, path.size() - last);
    dict.VALID = read_all_words(path, dict.CONTENT);
    return dict;
}

Dictionary get_dictionary(const string& name)
{
    return get_dictionary_wpath(string("resources/") + name);
}

set<tuple<string, string>> split_word(string word)
{
    set<tuple<string, string>> returnSet;
    for (int i = 0; i < word.length() + 1; ++i)
    {
        returnSet.insert(make_tuple(word.substr(0, i), word.substr(i, word.length())));
    }
    return returnSet;
}

vector<string> edit1(string word)
{
    char abc[] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z'
    };
    vector<string> mistakes;

    for (const auto& i : split_word(word))
    {
        string x = get<1>(i);
        // Letter is missing
        if (x.length() >= 1)
        {
            mistakes.push_back(get<0>(i) + x.substr(1));
        }
        else
        {
            mistakes.push_back(get<0>(i));
        }

        // 2 letters swapped
        if (x.length() >= 2)
        {
            string value;
            value = x.at(1);
            string value1;
            value1 = x.at(0);
            string temp = value + value1 + x.substr(2);
            mistakes.push_back(get<0>(i) + temp);
        }

        // Letter replaced
        if (x.length() >= 1)
        {
            for (char letter : abc)
            {
                mistakes.push_back(get<0>(i) + letter + x.substr(1));
            }
        }
        // Letter insertion
        for (char letter : abc)
        {
            mistakes.push_back(get<0>(i) + letter + x);
        }
    }
    return mistakes;
}

set<string> edit1_good(string& word, set<string>& all_words)
{
    set<string> returnSet;
    for (string mistake : edit1(word))
    {
        if (all_words.find(mistake) != all_words.end())
        {
            returnSet.insert(mistake);
        }
    }
    return returnSet;
}

template <typename T>
std::set<T> getUnion(const std::set<T>& a, const std::set<T>& b)
{
    std::set<T> result = a;
    result.insert(b.begin(), b.end());
    return result;
}

set<string> edit2_good(string& word, set<string>& all_words)
{
    set<string> returnSet;
    for (string mistake : edit1(word))
    {
        returnSet = getUnion<string>(returnSet, edit1_good(mistake, all_words));
    }

    return returnSet;
}

set<string> correct(string word, set<string>& word_list)
{
    toLowerCase(word);
    if (word_list.find(word) != word_list.end())
    {
        return set<string>{word};
    }
    auto basic_strings = edit1_good(word, word_list);
    if (basic_strings.size() >= 1)
    {
        return basic_strings;
    }
    auto good = edit2_good(word, word_list);
    if (good.size() >= 1)
    {
        return good;
    }
    return set<string>{word};
}

int main(int argc, char* argv[])
{
    string defaultDict = "german.dic";

    Dictionary dict;

    if (argc == 2)
    {
        string arg = argv[1];

        if (arg.substr(0, 8) == "-custom=")
        {
            dict = get_dictionary_wpath(arg.substr(8));
        }
        else if (arg.substr(0, 6) == "-name=")
        {
            dict = get_dictionary(arg.substr(6));
        }
    }
    else
    {
        dict = get_dictionary(defaultDict);
    }

    if (!dict.VALID) return 1;

    string input;
    cout <<
        "Write any word and the program will try to correct it based on the dictionary you specified in the main method!"
        << endl;
    cout << "Type 'exit' to exit the program!" << endl;

    while (input != "exit")
    {
        cout << "Input: ";
        getline(cin, input);
        if (input.empty())
        {
            continue;
        }

        if (input == "exit")
        {
            break;
        }

        toLowerCase(input);
        set<string> basic_strings = correct(input, dict.CONTENT);
        for (const string line : basic_strings)
        {
            cout << line << endl;
        }
    }
}