#include "nfa.h"
#include <sstream>
#include "utils.h"

#include <iostream>
#include <vector>
#include <string>

bool IfTransferedw(char chr)//符合\w规则
{
    if((chr>='0'&&chr<='9')||(chr>='A'&&chr<='Z')||(chr>='a'&&chr<='z')||chr=='_')
        return true;
    else
        return false;
}

bool IfAnchorb(const std::string str,const int index)//符合Anchor的b规则
{
    if(index==0)
            {
                if((str[index]>='0'&&str[index]<='9')||(str[index]>='A'&&str[index]<='Z')||(str[index]>='a'&&str[index]<='z')||str[index]=='_')
                    return true;
                else
                    return false;
            }
    else if(index==str.length())
    {
        if((str[index-1]>='0'&&str[index-1]<='9')||(str[index-1]>='A'&&str[index-1]<='Z')||(str[index-1]>='a'&&str[index-1]<='z')||str[index-1]=='_')
            return true;
        else
            return false;
    }
    else
    {
        if((str[index-1]>='0'&&str[index-1]<='9')||(str[index-1]>='A'&&str[index-1]<='Z')||(str[index-1]>='a'&&str[index-1]<='z')||str[index-1]=='_')
        {
            if((str[index]>='0'&&str[index]<='9')||(str[index]>='A'&&str[index]<='Z')||(str[index]>='a'&&str[index]<='z')||str[index]=='_')
                return false;
            else
                return true;
        }
        else
        {
            if((str[index]>='0'&&str[index]<='9')||(str[index]>='A'&&str[index]<='Z')||(str[index]>='a'&&str[index]<='z')||str[index]=='_')
                return true;
            else
                return false;
        }
    }
}


inline bool ObeytheRule(Rule rule, const std::string str, const int index ,const bool flag_m,const bool flag_s)//匹配函数
{
    
	
	if (rule.type == NORMAL)//普通转移
	{
        if(index>=str.length())
            return false;
        char chr=str[index];
        int ascii0 = static_cast<int>(chr);
		int ascii1 = rule.by[0];
		return ascii0 == ascii1;
	}
	else if (rule.type == RANGE)//范围转移
	{
        if(index>=str.length())
            return false;
        char chr=str[index];
	    int ascii0 = static_cast<int>(chr);
		int ascii1 = rule.by[0];
		int ascii2 = rule.to[0];
		return (ascii0 >= ascii1 && ascii0 <= ascii2);
	}
	else if (rule.type == SPECIAL)//特殊转移
	{
        if(index>=str.length())
            return false;
        char chr=str[index];
        int ascii0 = static_cast<int>(chr);
		if (rule.by[0] == 'd')
			return(ascii0 >= 48 && ascii0 <= 57);
		else if (rule.by[0] == 'D')
			return(ascii0 > 57 || ascii0 < 48);
		else if (rule.by[0] == 'w')
			return(IfTransferedw(chr));
		else if (rule.by[0] == 'W')
			return(!IfTransferedw(chr));
		else if (rule.by[0] == 's')
			return (chr == ' ' || chr == '\f' || chr == '\n' || chr == '\r' || chr == '\t' || chr == '\v');
		else if (rule.by[0] == 'S')
			return (chr!=' '&&chr!='\f'&&chr!='\n'&&chr!='\r'&&chr!='\t'&&chr!='\v');
        else if (rule.by[0]=='f')
            return chr=='\f';
        else if (rule.by[0]=='n')
            return chr=='\n';
        else if (rule.by[0]=='r')
            return chr=='\r';
        else if (rule.by[0]=='t')
            return chr=='\t';
        else if (rule.by[0]=='v')
            return chr=='\v';
		else if (rule.by[0] == '.')
        {
            if(flag_s)
                return true;
            else
                return (chr!='\n'&&chr!='\r');
        }
        else if (rule.by[0]=='A')
            return true;
	}
	else if (rule.type == EPSILON)//空转移
	{
        if(rule.by[0]=='^')
        {
            if(flag_m)
            {
                if(index==0||str[index-1]=='\n'||str[index-1]=='\r')
                    return true;
                else
                    return false;
            }
            else
            {
                if(index==0)
                    return true;
                else
                    return false;
            }
        }
        else if(rule.by[0]=='$')
        {
            if(flag_m)
            {
                if(index==str.length()||str[index]=='\n'||str[index]=='\r')
                    return true;
                else
                    return false;
            }
            else
            {
                if(index==str.length())
                    return true;
                else
                    return false;
            }
        }
        else if(rule.by[0]=='b')//index在\w与\W之间
        {

            return IfAnchorb(str,index);
        }
        else if(rule.by[0]=='B')
        {
            return !IfAnchorb(str,index);
        }
        else 
            return true;
        
    }
    return false;
	
}


NFA::NFA()//构造空NFA
{
    num_states = 1;
    is_final.push_back(false);
    std::vector<Rule> rule;
    rules.push_back(rule);
    is_group.push_back(false);
    group_begins.resize(1);
    group_ends.resize(1);   
}

NFA::NFA(enum RuleType type, std::string by, std::string to)//构造2状态NFA
{
    num_states = 2;
    is_final.push_back(false);
    is_final.push_back(true);
    std::vector<Rule> rule;
    rules.push_back(rule);
    rules.push_back(rule);
    Rule r;
    r.type = type;
    r.by = by;
    r.to = to;
    r.dst = 1;
    rules[0].push_back(r);

    is_group.push_back(false);
    is_group.push_back(false);
    group_begins.resize(2);
    group_ends.resize(2);
}

NFA::NFA(std::vector<int> my_set)//set构造NFA
{
    num_states = 2;
    is_final.push_back(false);
    is_final.push_back(true);
    is_group.push_back(false);
    is_group.push_back(false);
    group_begins.resize(2);
    group_ends.resize(2);
    std::vector<Rule> rule;
    rules.push_back(rule);
    rules.push_back(rule);
    for(int i = 0; i < my_set.size(); i += 2)
    {
        Rule temp_rule;
        temp_rule.dst = 1;
        if(my_set[i]==my_set[i+1])
        {
            temp_rule.type = NORMAL;
            temp_rule.by.push_back(my_set[i]);
        }
        else
        {
            temp_rule.type = RANGE;
            temp_rule.by.push_back(my_set[i]);
            temp_rule.to.push_back(my_set[i+1]);
        }
        rules[0].push_back(temp_rule);
    }

}

void NFA::print() {//打印NFA
    printf("num_states: %d\n", num_states);
    printf("is_final: ");
    for (int i = 0; i < num_states; i++)
        printf("%d ", (bool)is_final[i]);
    printf("\n");
    printf("rules:\n");
    for (int i = 0; i < num_states; i++) {
        for (auto it = rules[i].begin(); it < rules[i].end(); it++) {
            Rule rule = *it;
            printf("%d->%d %d %s %s\n", i, rule.dst, rule.type, rule.by.c_str(), rule.to.c_str());
        }
    }
    printf("is_group: ");
    for (int i = 0; i < num_states; i++)
        printf("%d ", (bool)is_group[i]);
    printf("\n");
    printf("group_begins: ");
    for (int i = 0; i < num_states; i++)
    {
        printf("%d ", i);
        for(auto it=group_begins[i].begin();it<group_begins[i].end();it++)
            printf("%d ",*it);
        printf("\n");
    }

    printf("\n");
    printf("group_ends: ");
    for (int i = 0; i < num_states; i++)
    {
        printf("%d ", i);
        for(auto it=group_ends[i].begin();it<group_ends[i].end();it++)
            printf("%d ",*it);
        printf("\n");
    }
    printf("\n");

}

bool NFA::DFS(std::string text, int state, Path &a,int &init,int &end,int i,std::vector<std::vector<bool>> &visited, std::vector<int> &id_begins, std::vector<int> &id_ends, std::vector<bool> &id_group)
{
	//printf("dfs %d %d\n", state, i);
	// if (i == text.length() )
	// {
	// 	if (is_final[state])
	// 	{
	// 		return true;
	// 	}

	// }
    if(is_final[state])//如果当前状态是终态
    {
        end=i;
        return true;
    }
    std::vector<int> id_begins_copy=id_begins;//备份
    std::vector<int> id_ends_copy=id_ends;
    std::vector<bool> id_group_copy=id_group;

    if(is_group[state])//如果当前状态是分组
    {
        for(int t=0;t<group_begins[state].size();t++)
        {
            id_group[group_begins[state][t]]=true;
            id_begins[group_begins[state][t]]=i;

        }
        for(int t=0;t<group_ends[state].size();t++)
        {
            id_group[group_ends[state][t]]=true;
            id_ends[group_ends[state][t]]=i;
        }
    }

	for (auto it = rules[state].begin(); it < rules[state].end(); it++)//遍历当前状态的所有规则
	{
		Rule rule = *it;

		if (rule.type == EPSILON)//如果当前状态是EPSILON转移
		{

			if (visited[rule.dst][i] == false&&ObeytheRule(rule, text,i,flag_m,flag_s))
			{   
               
                    if(state==0)
                    {
                        init=i;
                    }
                    a.consumes.push_back("");
                    a.states.push_back(rule.dst);
                    visited[rule.dst][i] = true;
                    if (DFS(text, rule.dst, a,init,end, i, visited,id_begins,id_ends,id_group))
                        return true;
                    a.states.pop_back();
                    a.consumes.pop_back();
                    //visited[rule.dst][i] = false;
                
			}

		}
		else//如果当前状态是普通转移
		{
			if (visited[rule.dst][i + 1] == false)//如果当前状态不是终态
			{
				if (i!=text.length()&&ObeytheRule(rule, text,i,flag_m,flag_s))
				{
					std::string str(1, text[i]);
					a.consumes.push_back(str);
					a.states.push_back(rule.dst);
					visited[rule.dst][i+1] = true;
					if (DFS(text, rule.dst, a, init ,end,i + 1, visited,id_begins,id_ends,id_group))
						return true;
					a.states.pop_back();
					a.consumes.pop_back();
                    //visited[rule.dst][i+1] = false;
				}
			}
		}
	}
    id_begins=id_begins_copy;//恢复
    id_ends=id_ends_copy;
    id_group=id_group_copy;
	
	return false;
}
/**
 * 在自动机上执行指定的输入字符串。
 * @param text 输入字符串
 * @return 若拒绝，请 return Path::reject(); 。若接受，请手工构造一个Path的实例并返回。
 */
Path NFA::exec(std::string text) {
    // TODO 请你完成这个函数
    if (num_states == 0)
		return Path::reject();
	else
	{
		Path path;
		path.states.push_back(0);
		std::vector < std::vector < bool >> visited(num_states, std::vector<bool>(text.length()+1,false));
        int init=0,end=0;
        std::vector<int> id_begins(group_num), id_ends(group_num);
        std::vector<bool> is_group(group_num);
		if (DFS(text, 0, path,init , end,0,visited,id_begins,id_ends,is_group))
			return path;
	}
    return Path::reject();
}

/**
 * 将Path转为（序列化为）文本的表达格式（以便于通过stdout输出）
 * 你不需要理解此函数的含义、阅读此函数的实现和调用此函数。
 */
std::ostream &operator<<(std::ostream &os, Path &path) {
    if (!path.states.empty()) {
        if (path.consumes.size() != path.states.size() - 1)
            throw std::runtime_error("Path的len(consumes)不等于len(states)-1！");
        for (int i = 0; i < path.consumes.size(); ++i) {
            os << path.states[i] << " " << path.consumes[i] << " ";
        }
        os << path.states[path.states.size() - 1];
    } else os << std::string("Reject");
    return os;
}

/**
 * 从自动机的文本表示构造自动机
 * 你不需要理解此函数的含义、阅读此函数的实现和调用此函数。
 */
NFA NFA::from_text(const std::string &text) {
    NFA nfa = NFA();
    bool reading_rules = false;
    std::istringstream ss(text);
    std::string line, type;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        if (line.find("type:") == 0) {
            type = strip(line.substr(5));
            continue;
        }
        if (type != "nfa") throw std::runtime_error("输入文件的类型不是nfa！");
        if (line.find("states:") == 0) {
            nfa.num_states = std::stoi(line.substr(7));
            for (int i = 0; i < nfa.num_states; ++i) {
                nfa.rules.emplace_back();
                nfa.is_final.push_back(false);
            }
            continue;
        } else if (line.find("final:") == 0) {
            if (nfa.num_states == 0) throw std::runtime_error("states必须出现在final和rules之前!");
            std::istringstream ss2(line.substr(6));
            int t;
            while (true) {
                ss2 >> t;
                if (!ss2.fail()) nfa.is_final[t] = true;
                else break;
            }
            reading_rules = false;
            if (ss2.eof()) continue;
        } else if (line.find("rules:") == 0) {
            if (nfa.num_states == 0) throw std::runtime_error("states必须出现在final和rules之前!");
            reading_rules = true;
            continue;
        } else if (line.find("input:") == 0) {
            reading_rules = false;
            continue;
        } else if (reading_rules) {
            auto arrow_pos = line.find("->"), space_pos = line.find(' ');
            if (arrow_pos != std::string::npos && space_pos != std::string::npos && arrow_pos < space_pos) {
                int src = std::stoi(line.substr(0, arrow_pos));
                int dst = std::stoi(line.substr(arrow_pos + 2, space_pos - (arrow_pos + 2)));
                auto content = line.substr(space_pos + 1);
                bool success = true;
                while (success && !content.empty()) {
                    auto p = content.find(' ');
                    if (p == std::string::npos) p = content.size();
                    else if (p == 0) p = 1; // 当第一个字母是空格时，说明转移的字符就是空格。于是假定第二个字母也是空格（如果不是，会在后面直接报错）
                    Rule rule{dst};
                    if (p == 3 && content[1] == '-') {
                        rule.type = RANGE;
                        rule.by = content[0];
                        rule.to = content[2];
                    } else if (p == 2 && content[0] == '\\') {
                        if (content[1] == 'e') rule.type = EPSILON;
                        else {
                            rule.type = SPECIAL;
                            rule.by = content[1];
                        }
                    } else if (p == 1 && (p >= content.length() || content[p] == ' ')) {
                        rule.type = NORMAL;
                        rule.by = content[0];
                    } else success = false;
                    nfa.rules[src].push_back(rule);
                    content = content.substr(std::min(p + 1, content.size()));
                }
                if (success) continue;
            }
        }
        throw std::runtime_error("无法parse输入文件！失败的行： " + line);
    }
    if (!ss.eof()) throw std::runtime_error("无法parse输入文件！(stringstream在getline的过程中发生错误)");;
    return nfa;
}
