//
// Created by machen on 2021/9/14.
//

#include "code.h"

bool is_special_char(const char c){
    //  isalnum(c) == 0表示不是字母也不是数字
    return isalnum(c) == 0 && c != '_';
}

void code::trie_tree::_insert(int index, std::string &phrase, code::Node *node, code::phrase_type _type){
    if(index >= phrase.length()){
        node->this_type = _type;
        return;
    }
    // 若不在树内
    if(node->child.find(phrase[index]) == node->child.end()){
        node->child.insert(std::make_pair(phrase[index], new code::Node(phrase[index])));
    }
    _insert(index + 1, phrase, node->child[phrase[index]], _type);
}

void code::trie_tree::insert(const QString &filePath, code::phrase_type _type){

    QFile file(filePath);
    //--打开文件成功
    if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            std::string phrase = textStream.readLine().toStdString();
            if(phrase.empty())
                continue;
            // 插入根结点
            if(root->child.find(phrase[0]) == root->child.end())
                root->child.insert(std::make_pair(phrase[0], new Node(phrase[0])));

            _insert(1, phrase, root->child[phrase[0]], _type);
        }

    }
    else	//---打开文件失败
    {
        QMessageBox ::information(NULL, NULL, "样例文件丢失");
    }
}

std::pair<std::string, code::phrase_type>  code::trie_tree::find_char(const char &phrase){
    // 当前字符无法继续在字典树中游走
    if(now_ptr->child.find(phrase) == now_ptr->child.end()){
        phrase_type output_type = now_ptr->this_type;
        std::string tmp = output;
        output.clear();
        // 初始化
        if(root->child.find(phrase) != root->child.end()){
            now_ptr = root->child[phrase];
            output.push_back(phrase);
        }
        return std::make_pair(tmp, output_type);
    }
    else{
        now_ptr = now_ptr->child[phrase];
        output.push_back(now_ptr->this_char);
        return std::make_pair("", NONE);
    }
}

bool is_identifier(const std::string& phrase){
    if(phrase.empty())
        return false;
    if(!isalpha(*phrase.begin()) && *phrase.begin() != '_')
        return false;

    for (int i = 1; i < phrase.length(); ++i) {
        if(is_special_char(phrase[i]))
            return false;
    }
    return true;

}

bool is_numbers(const std::string& phrase){
    if(phrase.empty() || (!isnumber(*phrase.begin()) && *phrase.begin() != '-')){
        return false;
    }
    bool has_decimal  = false;
    bool has_e = false;
    for(auto iter = phrase.begin(); iter < phrase.end();++iter) {
        if (*iter == '.') {
            if (!has_decimal && isnumber(*(iter - 1))) {
                has_decimal = true;
            } else
                return false;
        } else if (*iter == 'e') {
            if (!has_e && isnumber(*(iter - 1))) {
                has_e = true;
            } else
                return false;
        } else if (!isnumber(*iter))
            break;
    }
    return true;
}
void addResult(std::vector<std::unordered_map<std::string, int>>& result, const code::phrase_type _type,const std::string& _string){
    if(result[_type].find(_string) == result[_type].end())
        result[_type][_string] = 0;

    result[_type][_string] += 1;
}

/**
 * 识别
 */
std::vector<std::unordered_map<std::string, int>> code::identify(const std::string& file){
    code::trie_tree trieTree;
    trieTree.insert(":/file/reference/file/operators_list", code::OPERATOR);
    trieTree.insert(":/file/reference/file/keywords_list", code::KEYWORD);
    trieTree.insert(":/file/reference/file/others_list", code::OTHER);
    trieTree.insert(":/file/reference/file/escape_chars_list", code::ESCAPE_CHAR);
    std::vector<std::unordered_map<std::string, int>> result(10);
    Status status = NORMAL;
    std::string cache;
    for( auto iter = file.begin(); iter <= file.end(); ++iter){
        // 遍历该行, 每一个字符
        auto output = trieTree.find_char(*iter);
        // 触发 //
        if(output.first == "//"){
            if(status == NORMAL){
                addResult(result, output.second, output.first);
                status = SINGLE_NOTE;
            }
        }
        // 触发 /* */
        else if(output.first == "/*"){
            if(status == NORMAL){
                addResult(result, output.second, output.first);
                status = MULTI_NOTE;
            }
        }
        else if(output.first == "*/"){
            if(status == MULTI_NOTE){
                status = NORMAL;
            }
        }
        // 触发 '
        else if(output.first == "\'"){
            if(status == NORMAL)
                status = CHAR;
            else if(status == CHAR){
                status = NORMAL;
                addResult(result, output.second, output.first);
            }

        }
        // 触发"
        else if(output.first == "\""){
            if(status == NORMAL)
                status = STR;
            else if(status == STR){
                status = NORMAL;
                addResult(result, output.second, output.first);
            }

        }

        // 处理 特殊情况
        if(status != NORMAL || output.second == ESCAPE_CHAR){
            if(*iter == '\n' && status == SINGLE_NOTE){
                status = NORMAL;
                // 需要重置 output
                output.second = NONE;
            }
            else
                continue;
        }

        // 将数字也装入内
        if(!is_special_char(*iter)){
            cache.push_back(*iter);
        }
        // iter为数字不会触发
        if(is_special_char(*iter)){
            // 数字的分隔特殊处理
            if((*iter == '.' || *iter == '-') && isnumber(*(iter+1))){
                cache.push_back(*iter);
                continue;
            }
            // 判断cache是否为数字
            if(is_numbers(cache)){
                addResult(result, NUMBER, cache);
            }
            else if(output.second == NONE){
                // 检查缓存是否为标识符
                if(is_identifier(cache)){
                    addResult(result, IDENTIFIER, cache);
                }
            }
            cache.clear();
        }
        if(output.second != NONE){
            if(!output.first.empty())
                addResult(result, output.second, output.first);
        }
        // 进行换行
        if(*iter == '\n'){
            cache.clear();
        }
    }

    return result;
}
