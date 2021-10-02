#include <iostream>
#include <unordered_map>
#include <utility>
#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QString>
namespace code {
    enum phrase_type: int {NONE = 0, KEYWORD, OPERATOR, OTHER, IDENTIFIER, NUMBER, ESCAPE_CHAR};
    enum Status: int {NORMAL=0, STR, CHAR, SINGLE_NOTE, MULTI_NOTE};
    class Node {
    public:
        char this_char;
        phrase_type this_type;
        std::unordered_map<char, Node *> child;
        explicit Node(char _this_char, phrase_type _type = NONE) : this_char(_this_char), this_type(_type) {};
    };
    /**
     * 特殊符号字典树
     * 若has_output为true, 则调用其中output的值即可
     */
class trie_tree {
    private:
        Node *root;
        Node *now_ptr;
        std::string output;
        static trie_tree* self;
        void _insert(int index, std::string &phrase, Node *node, code::phrase_type _type);
        explicit trie_tree(): root(new Node(' ')), now_ptr(root){};
    public:
        static trie_tree* get_trie_tree();
        void insert(const QString &filePath, code::phrase_type _type);
        std::pair<std::string, code::phrase_type>   find_char(const char &phrase);
        // 仅能识别出关键字, 运算符和其他符号
    };
    std::vector<std::unordered_map<std::string, int>> identify(const std::string& file);
}
