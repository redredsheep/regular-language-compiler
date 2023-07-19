#include "regex.h"

/**
 * 注：如果你愿意，你可以自由的using namespace。
 */
std::bitset<128> computeUnion(const std::bitset<128>& set1, const std::bitset<128>& set2) {
    return set1 | set2;
}

std::bitset<128> computeComplement(const std::bitset<128>& set) {
    return ~set;
}

std::vector<int> convertToSet(const std::bitset<128>& set) {//将二进制集合转换为字符区间
    std::vector<int> result;
    bool isRange = false;
    int rangeStart = 0;

    for (int i = 0; i < 128; i++) {
        if (set[i]) {
            if (!isRange) {
                isRange = true;
                rangeStart = i;
            }
        } else {
            if (isRange) {
                isRange = false;
                result.push_back(rangeStart);
                result.push_back(i - 1);
            }
        }
    }

    if (isRange) {
        result.push_back(rangeStart);
        result.push_back(127);
    }

    return result;
}

std::bitset<128> convertCharToBinarySet(char input) {//将字符转换为128位的二进制集合
    std::bitset<128> binarySet;

    int ascii = static_cast<int>(input);
    if (ascii >= 0 && ascii < 128) {
        binarySet.set(ascii, true);
    }

    return binarySet;
}

// 将字符区间的 ASCII 码转换为 128 位的二进制集合
std::bitset<128> convertStrToBinarySet(const std::string& input) {
    if (input.length() != 3 || input[1] != '-') {
        // 无效的输入格式
        return 0;
    }

    char startChar = input[0];
    char endChar = input[2];
    std::bitset<128> binarySet;

    for (int ascii = static_cast<int>(startChar); ascii <= static_cast<int>(endChar); ascii++) {
        if (ascii >= 0 && ascii < 128) {
            binarySet.set(ascii, true);
        }
    }

    return binarySet;
}

// 将元字符转换为 129 位的二进制集合
std::bitset<128> convertMetaCharToBinarySet(const std::string& metaChar) {
    std::bitset<128> binarySet;

    char meta = metaChar[1];

    if (meta == 'd') {
        for (int i = 48; i <= 57; i++) {
            binarySet.set(i, true);
        }
    } else if (meta == 'D') {
        for (int i = 0; i < 48 || (i > 57 && i < 128); i++) {
            binarySet.set(i, true);
        }
    } else if (meta == 'w') {
        for (int i = 48; i <= 57; i++) {
            binarySet.set(i, true);
        }
        for (int i = 65; i <= 90; i++) {
            binarySet.set(i, true);
        }
        for (int i = 97; i <= 122; i++) {
            binarySet.set(i, true);
        }
        binarySet.set(95, true);
    } else if (meta == 'W') {
        for (int i = 0; i < 48 || (i > 57 && i < 65) || (i > 90 && i < 95) || (i == 96) || (i > 122 && i < 128); i++) {
            binarySet.set(i, true);
        }
    } else if (meta == 's') {
        binarySet.set(' ', true);
        binarySet.set('\f', true);
        binarySet.set('\r', true);
        binarySet.set('\t', true);
        binarySet.set('\v', true);
    } else if (meta == 'S') {
        for (int i = 0; i < 128; i++) {
            if (i != ' ' && i != '\f' && i != '\r' && i != '\t' && i != '\v') {
                binarySet.set(i, true);
            }
        }
    } else if (meta == '.') {
        binarySet.set();
        binarySet.set('\n', false);
        binarySet.set('\r', false);
  
    }

    return binarySet;
}

void AddStateId(NFA &a,int addition)//提升状态编号
{
    for(int i=0;i<a.num_states;i++)
    {
        for(int j=0;j<a.rules[i].size();j++)
        {
            a.rules[i][j].dst+=addition;
        }
    }
    return;

}

void InsertInitial(NFA &a)//添加初态
{
    AddStateId(a,1);
    a.is_final.insert(a.is_final.begin(),false);
    std::vector<Rule> temp_rules;
    a.rules.insert(a.rules.begin(),temp_rules);
    a.is_group.insert(a.is_group.begin(),false);
    a.group_begins.insert(a.group_begins.begin(),std::vector<int>());
    a.group_ends.insert(a.group_ends.begin(),std::vector<int>());
    Rule temp_rule;
    temp_rule.type = EPSILON;
    temp_rule.dst = 1;
    a.rules[0].push_back(temp_rule);
    a.num_states+=1;
    return;
}


void AddFinalEpsilon_Just(NFA &a)//添加终态
{
    a.is_final.push_back(true);
    std::vector<Rule> temp_rules;
    a.rules.push_back(temp_rules);
    a.is_final[a.num_states-1]=false;
    a.group_begins.push_back(std::vector<int>());
    a.group_ends.push_back(std::vector<int>());
    a.is_group.push_back(false);
    a.num_states+=1;
    return;

}

void AddFinalEpsilon_MultiEnd(NFA &a)//添加终态
{
    a.is_final.push_back(true);
    std::vector<Rule> temp_rules;
    a.rules.push_back(temp_rules);
    Rule temp_rule;
    temp_rule.type = EPSILON;
    temp_rule.dst = a.num_states;
    a.rules[a.num_states-1].push_back(temp_rule);
    a.is_final[a.num_states-1]=false;
    a.group_begins.push_back(std::vector<int>());
    a.group_ends.push_back(std::vector<int>());
    a.is_group.push_back(false);
    a.num_states+=1;
    for(int i=0;i<a.num_states-1;i++)
    {
        if(a.is_final[i]==true)
        {
            a.rules[i].push_back(temp_rule);
            a.is_final[i]=false;
        }
    }
    return;

}
void LiftGroup(NFA &a)//提升组别信息
{
    for(int i=0;i<a.group_begins.size();i++)
    {
        for(int j=0;j<a.group_begins[i].size();j++)
        {
            a.group_begins[i][j]+=1;
        }
    }
    for(int i=0;i<a.group_ends.size();i++)
    {
        for(int j=0;j<a.group_ends[i].size();j++)
        {
            a.group_ends[i][j]+=1;
        }
    }
    a.group_begins[0].insert(a.group_begins[0].begin(),0);
    for(int i=0;i<a.num_states;i++)
    {
        if(a.is_final[i]==true)
        {
            a.group_ends[i].push_back(0);
        }
    }
    a.group_num+=1;
    return;
}

void MergeGroupId(NFA &a, NFA b)//合并组别信息
{
    a.group_begins.resize(a.num_states+b.num_states);
    a.group_ends.resize(a.num_states+b.num_states);
    a.is_group.resize(a.num_states+b.num_states);
    if(b.group_num>0)
    {
        for(int i=0;i<b.group_begins.size();i++)
        {
            for(int j=0;j<b.group_begins[i].size();j++)
            {
                b.group_begins[i][j]+=a.group_num;
                a.group_begins[a.num_states+i].push_back(b.group_begins[i][j]);
                a.is_group[a.num_states+i]=true;
            }
            
        }
        for(int i=0;i<b.group_ends.size();i++)
        {
            for(int j=0;j<b.group_ends[i].size();j++)
            {
                b.group_ends[i][j]+=a.group_num;
                a.group_ends[a.num_states+i].push_back(b.group_ends[i][j]);
                a.is_group[a.num_states+i]=true;
            }
        }
        a.group_num+=b.group_num;
    }
    return ;
}

void mergeGroupId_range(NFA &a,NFA b)//合并自动机状态特殊情况
{
    for(int i=0;i<b.num_states;i++)
    {
        a.group_begins.push_back(b.group_begins[i]);
        a.group_ends.push_back(b.group_ends[i]);
        a.is_group.push_back(b.is_group[i]);
    }
    return;
}
void mergeId(NFA &a,NFA b)//合并自动机状态
{
    for (int i = 0; i < b.num_states; i++) {
        a.is_final.push_back(b.is_final[i]);
    }
    AddStateId(b,a.num_states);
    for (int i = 0; i < b.num_states; i++) {
        a.rules.push_back(b.rules[i]);
    }

    return;

}


void mergeNFA_OR (NFA &a, NFA b) {//并联自动机
    
    
    mergeId(a,b);
    Rule temp_rule;
    temp_rule.type = EPSILON;
    temp_rule.dst = a.num_states;
    a.rules[0].push_back(temp_rule);

    MergeGroupId(a,b);
    a.num_states += b.num_states;
    AddFinalEpsilon_MultiEnd(a);
    return ;
}

void mergeNFA_AND (NFA &a, NFA b,bool IfRange) {//连接自动机，Range特殊处理
    // a.group_begins.resize(a.num_states+b.num_states);
    // a.group_ends.resize(a.num_states+b.num_states);
    // a.is_group.resize(a.num_states+b.num_states);
    
    mergeId(a,b);
    if(IfRange)
    {
        mergeGroupId_range(a,b);
    }
    else
    {
        MergeGroupId(a,b);
    }
    for(int i=0;i<a.num_states;i++)
    {
        if(a.is_final[i]==true)
        {
            Rule temp_rule;
            temp_rule.type = EPSILON;
            temp_rule.dst = a.num_states;
            a.rules[i].push_back(temp_rule);
            a.is_final[i]=false;
        }
    }



    a.num_states += b.num_states;
    return ;
}
void mergeNFA_QUESTION (bool IfGreedy ,NFA &a) {//问号闭包
    InsertInitial(a);
    AddFinalEpsilon_MultiEnd(a);
    Rule temp_rule;
    temp_rule.type = EPSILON;
    temp_rule.dst = a.num_states-1;
    if(IfGreedy)
    {
        a.rules[0].push_back(temp_rule);
    }
    else
    {
        a.rules[0].insert(a.rules[0].begin(),temp_rule);
    }
 
    return ;
}

void mergeNFA_STAR (bool IfGreedy ,NFA &a) {//星闭包
    mergeNFA_QUESTION(IfGreedy,a);
    Rule temp_rule;
    temp_rule.type = EPSILON;
    temp_rule.dst = 0;
    AddFinalEpsilon_Just(a); 
    if(IfGreedy)
    {
        a.rules[a.num_states-2].push_back(temp_rule);
        temp_rule.dst = a.num_states-1;
        a.rules[a.num_states-2].push_back(temp_rule);
    }
    else
    {
        temp_rule.dst = a.num_states-1;
        a.rules[a.num_states-2].push_back(temp_rule);
        temp_rule.dst = 0;
        a.rules[a.num_states-2].push_back(temp_rule);
    }
    return ;
}

void mergeNFA_PLUS (bool IfGreedy ,NFA &a) {//加闭包
    AddFinalEpsilon_MultiEnd(a);
    Rule temp_rule;
    temp_rule.type = EPSILON;
    temp_rule.dst = 0;
    a.rules[a.num_states-1].push_back(temp_rule);
    AddFinalEpsilon_Just(a);
    temp_rule.dst = a.num_states-1;
    if(IfGreedy)
    {
        a.rules[a.num_states-2].push_back(temp_rule);
    }
    else
    {
        a.rules[a.num_states-2].insert(a.rules[a.num_states-2].begin(),temp_rule);
    }
    return ;
}

void mergeNFA_RANGE(bool IfGreedy ,int min,int max,NFA &a) {//合并min到max次
    NFA b=a;
    if(min==0)
    {
        mergeNFA_QUESTION(IfGreedy,a);
        min=1;
    }
    for(int i=1;i<min;i++)
    {
        mergeNFA_AND(a,b,true);
    }
    if(max==-1)
    {
        mergeNFA_STAR(IfGreedy,b);
        mergeNFA_AND(a,b,true);
    }
    else
    {
        for(int i=min;i<max;i++)
        {
            mergeNFA_QUESTION(IfGreedy,b);
            mergeNFA_AND(a,b,true);
        }
    }
    return ;
}
NFA compileExpression(regexParser::ExpressionContext *ctx);
NFA compileExpressionItem(regexParser::ExpressionItemContext *ctx);
NFA compileNormalItem(regexParser::NormalItemContext *ctx) ;
NFA compileQuantifier(regexParser::QuantifierContext *ctx, NFA temp_NFA) ;
NFA compileSingle(regexParser::SingleContext *ctx) ;
NFA compilechar(regexParser::CharContext *ctx);
NFA compilecharacterClass(regexParser::CharacterClassContext *ctx) ;
NFA compileAnyCharacter(antlr4::tree::TerminalNode *ctx) ;
NFA compilecharacterGroup(regexParser::CharacterGroupContext *ctx);
NFA compileSet(std::vector<int> my_set);
NFA compileGroup(regexParser::GroupContext *ctx) ;

NFA compileRegex(regexParser::RegexContext *ctx) {
    auto expressions = ctx->expression(); // 获得所有expression
    NFA my_NFA; // 用于存放编译结果
    //my_NFA.is_final[0]=true;

    for (auto expression : expressions) {
        NFA temp_NFA = compileExpression(expression); // 编译expression
        mergeNFA_OR(my_NFA, temp_NFA); // 合并NFA
        // TODO 将xxx合并到yyy中
    }
    return my_NFA;
}

NFA compileExpression(regexParser::ExpressionContext *ctx) {
    auto expressionItems = ctx->expressionItem(); // 获得所有expressionItem
    NFA my_NFA; // 用于存放编译结果
    my_NFA.is_final[0]=true;

    for (auto expressionItem : expressionItems) {
        NFA temp_NFA = compileExpressionItem(expressionItem); // 编译expressionItem
        mergeNFA_AND(my_NFA, temp_NFA,false); // 合并NFA
        // TODO 将xxx合并到yyy中
    }
    return my_NFA;
}

NFA compileExpressionItem(regexParser::ExpressionItemContext *ctx) {
    if(ctx->normalItem()!=NULL)
    {
        auto normalItem = ctx->normalItem(); // 获得normalItem
        auto quantifier = ctx->quantifier(); // 获得quantifier
        NFA temp_NFA = compileNormalItem(normalItem); // 编译atom
        if (quantifier) {
            
            temp_NFA = compileQuantifier(quantifier, temp_NFA); // 编译quantifier
            
        }
        return temp_NFA;
    
    }
    else 
    {
        auto anchor = ctx->anchor(); // 获得anchor
        if(anchor->AnchorStartOfString())
        {
            return NFA(EPSILON,"^","");
        }
        else if(anchor->AnchorEndOfString())
        {
            return NFA(EPSILON,"$","");
        }
        else if(anchor->AnchorWordBoundary())
        {
            return NFA(EPSILON,"b","");
        }
        else if(anchor->AnchorNonWordBoundary())
        {
            return NFA(EPSILON,"B","");
        }
    }

}

NFA compileNormalItem(regexParser::NormalItemContext *ctx) {//编译normalItem

    auto single = ctx->single(); // 获得single
    auto group = ctx->group(); // 获得group
    NFA temp_NFA;
    if(ctx->single())
    {
        temp_NFA = compileSingle(single); // 编译single
    }
    if(ctx->group())
    {
        temp_NFA = compileGroup(group); // 编译group
    }
    
    return temp_NFA;
}

NFA compileQuantifier(regexParser::QuantifierContext *ctx, NFA temp_NFA) {//编译quantifier
    auto quantifier = ctx-> quantifierType(); // 获得quantifier
    if (quantifier->ZeroOrOneQuantifier())
    {
        if(ctx->lazyModifier())
            mergeNFA_QUESTION(false,temp_NFA);
        else
            mergeNFA_QUESTION(true,temp_NFA);
    }
    else if (quantifier->ZeroOrMoreQuantifier())
    {
        if(ctx->lazyModifier())
            mergeNFA_STAR(false,temp_NFA);
        else
            mergeNFA_STAR(true,temp_NFA);
    }
    else if (quantifier->OneOrMoreQuantifier())
    {
        if(ctx->lazyModifier())
            mergeNFA_PLUS(false,temp_NFA);
        else
            mergeNFA_PLUS(true,temp_NFA);
    }
    else if(quantifier->rangeQuantifier())//范围量词
    {
        auto rangeQuantifier = quantifier->rangeQuantifier();
        int min,max;
        min=std::stoi(rangeQuantifier->rangeQuantifierLowerBound()->getText());
        if(rangeQuantifier->rangeQuantifierUpperBound()==NULL&&rangeQuantifier->rangeDelimiter()==NULL)
        {
            max=min;
        }
        else if(rangeQuantifier->rangeQuantifierUpperBound()==NULL&&rangeQuantifier->rangeDelimiter()!=NULL)
        {
            max=-1;
        }
        else if(rangeQuantifier->rangeQuantifierUpperBound()!=NULL&&rangeQuantifier->rangeDelimiter()==NULL)
        {
            max=-1;
        }
        else
        {
            max=std::stoi(rangeQuantifier->rangeQuantifierUpperBound()->getText());
        }
        if(ctx->lazyModifier())
            mergeNFA_RANGE(false,min,max,temp_NFA);
        else
            mergeNFA_RANGE(true,min,max,temp_NFA);
    }

    return temp_NFA;
}

NFA compileSingle(regexParser::SingleContext *ctx) {//编译single
    NFA temp_NFA;
    auto character=ctx->char_();
    auto characterClass=ctx->characterClass();
    auto anycharactor=ctx->AnyCharacter();
    auto characterGroup=ctx->characterGroup();
    
    if(character)//如果是char
    {
        temp_NFA = compilechar(character);
    }
    else if(characterClass)//如果是characterClass
    {
        temp_NFA = compilecharacterClass(characterClass);
    }
    else if(anycharactor)// 如果是anycharactor
    {
        temp_NFA = compileAnyCharacter(anycharactor);
    }
    else if(characterGroup)//如果是characterGroup
    {
        temp_NFA = compilecharacterGroup(characterGroup);
    }
    return temp_NFA;

}

NFA compilechar(regexParser::CharContext *ctx) {//编译char
    if(ctx->EscapedChar())
    {
        std::string str;
        str.push_back(ctx->getText()[1]);
        if(ctx->getText()[1]=='f'||ctx->getText()[1]=='n'||ctx->getText()[1]=='r'||ctx->getText()[1]=='t'||ctx->getText()[1]=='v')
        {
            NFA temp_NFA(SPECIAL,str,str);
            return temp_NFA;
        }
        else 
        {
            NFA temp_NFA(NORMAL,str,str);
            return temp_NFA;
        }

    }
    else
    {
        std::string str;
        str.push_back(ctx->getText()[0]);
        NFA temp_NFA(NORMAL,str,str);
        return temp_NFA;
    }
}

NFA compilecharacterClass(regexParser::CharacterClassContext *ctx) {//编译characterClass
    
    std::string str;
    str.push_back(ctx->getText().back());
    NFA temp_NFA(SPECIAL,str,str);
    return temp_NFA;
}

NFA compileAnyCharacter(antlr4::tree::TerminalNode *ctx) {
    std::string str;
    str.push_back('.');
    NFA temp_NFA(SPECIAL,str,str);
    return temp_NFA;
}

NFA compilecharacterGroup(regexParser::CharacterGroupContext *ctx) {//编译characterGroup
    bool isNegative = false;
    if(ctx->characterGroupNegativeModifier())
        isNegative = true;
    auto charactergroupitems = ctx->characterGroupItem();
    NFA temp_NFA;
    std::bitset<128> my_bitset;//初始化一个bitset
    
    for(auto item:charactergroupitems)//遍历所有的item
    {
        if(item->charInGroup())
        {
            char chr;
            if(!item->charInGroup()->EscapedChar())
                chr=item->getText()[0];
            else
            {
                chr=item->getText()[1];
            }
            std::bitset<128> temp_bitset=convertCharToBinarySet(chr);
            my_bitset=my_bitset|temp_bitset;

        }
        else if (item->characterClass())
        {
            std::string str=item->getText();
            std::bitset<128> temp_bitset=convertMetaCharToBinarySet(str);
            my_bitset=my_bitset|temp_bitset;


        }
        else if(item->characterRange())
        {
            std::string str=item->getText();
            std::bitset<128> temp_bitset=convertStrToBinarySet(str);
            my_bitset=my_bitset|temp_bitset;
        }
    }
    if(isNegative)//如果是negative的，取反
    {
        my_bitset=~my_bitset;
    }
    std::vector<int> my_set=convertToSet(my_bitset);
    temp_NFA = compileSet(my_set);
    return temp_NFA;
}

NFA compileSet(std::vector<int> my_set) {
    return NFA(my_set);
}
NFA compileGroup(regexParser::GroupContext *ctx) {
    auto regex = ctx->regex(); // 获得regex
    NFA temp_NFA = compileRegex(regex);
    if(ctx->groupNonCapturingModifier()==NULL)
        LiftGroup(temp_NFA);
    return temp_NFA;
}

/**
 * 编译给定的正则表达式。
 * 具体包括两个过程：解析正则表达式得到语法分析树（这步已经为你写好，即parse方法），
 * 和在语法分析树上进行分析（遍历），构造出NFA（需要你完成的部分）。
 * 在语法分析树上进行分析的方法，可以是直接自行访问该树，也可以是使用antlr的Visitor机制，详见作业文档。
 * 你编译产生的结果，NFA应保存在当前对象的nfa成员变量中，其他内容也建议保存在当前对象下（你可以自由地在本类中声明新的成员）。
 * @param pattern 正则表达式的字符串
 * @param flags 正则表达式的修饰符
 */
void Regex::compile(const std::string &pattern, const std::string &flags) {//这里的pattern是正则表达式，flags是修饰符
    regexParser::RegexContext *tree = Regex::parse(pattern); // 这是语法分析树
    
    nfa=compileRegex(tree);
    for(int i=0;i<flags.size();i++)
    {
        if(flags[i]=='s')
        {
            nfa.flag_s=true;
        }
        else if(flags[i]=='m')
        {
            nfa.flag_m=true;
        }
    }
    // TODO 请你将在上次实验的内容粘贴过来，在其基础上进行修改。
}


/**
 * 在给定的输入文本上，进行正则表达式匹配，返回匹配到的第一个结果。
 * 匹配不成功时，返回空vector( return std::vector<std::string>(); ，或使用返回初始化列表的语法 return {}; )；
 * 匹配成功时，返回一个std::vector<std::string>，其中下标为0的元素是匹配到的字符串，
 * 下标为i(i>=1)的元素是匹配结果中的第i个分组。
 * @param text 输入的文本
 * @return 如上所述
 */


matchStr Regex::matchIndex(std::string text,int beginning)//返回匹配到的第一个结果
{

    //nfa.print();
    Path path;
    path.states.push_back(0);
    int init=0,end=0;
    std::vector < std::vector < bool >> visited(nfa.num_states, std::vector<bool>(text.length()+1,false));
    std::vector<int> id_begins(nfa.group_num), id_ends(nfa.group_num);
    std::vector<bool> if_group(nfa.group_num);
    matchStr my_match;
    if(nfa.DFS(text,0,path,init,end,beginning,visited, id_begins, id_ends, if_group))
    {
        std::vector<std::string> result;
        result.push_back(text.substr(init,end-init));
        for(int i=0;i<if_group.size();i++)
        {
            //std::cout<<if_group[i]<<std::endl;
        }
        for(int i=0;i<nfa.group_num;i++)
        {
            if(if_group[i])
            {
                result.push_back(text.substr(id_begins[i],id_ends[i]-id_begins[i]));
            }
            else
            {
                result.push_back("");
            }
        }
        my_match.if_match=true;
        my_match.matchstr=result;
        my_match.init=init;
        my_match.end=end;
        return my_match;
        //std::cout<<path;
    }
    else
    {
        my_match.if_match=false;
        return my_match;
    }
}
std::vector<std::string> Regex::match(std::string text) {
    // TODO 请你将在上次实验的内容粘贴过来，在其基础上进行修改。
        InsertInitial(nfa);
    Rule temp_rule;
    temp_rule.type=SPECIAL;
    temp_rule.dst=0;
    temp_rule.by.push_back('A');
    nfa.rules[0].push_back(temp_rule);
    matchStr my_match=matchIndex(text,0);
    if(my_match.if_match)//匹配成功
    {
        //for(int i=0;i<my_match.matchstr.size();i++)
        //{
            //std::cout<<my_match.matchstr[i]<<std::endl;
        //}
        return my_match.matchstr;
    }
    else
    {
        return {};
    }
}

std::vector<matchStr> Regex::matchAllIndex(std::string text)//返回匹配到的所有结果
{

    std::vector<matchStr> matchallIndex;
    matchStr my_match=matchIndex(text,0);
    while(my_match.if_match)
    {
        matchallIndex.push_back(my_match);
        my_match=matchIndex(text,my_match.end);
    }
    return matchallIndex;


}
/**
 * 在给定的输入文本上，进行正则表达式匹配，返回匹配到的**所有**结果。
 * 匹配不成功时，返回空vector( return std::vector<std::string>(); ，或使用返回初始化列表的语法 return {}; )；
 * 匹配成功时，返回一个std::vector<std::vector<std::string>>，其中每个元素是每一个带分组的匹配结果，其格式同match函数的返回值（详见上面）。
 * @param text 输入的文本
 * @return 如上所述
 */
std::vector<std::vector<std::string>> Regex::matchAll(std::string text) {
    // TODO 请你完成这个函数
    //std::cout<<nfa.flag_m<<std::endl;
        InsertInitial(nfa);
    Rule temp_rule;
    temp_rule.type=SPECIAL;
    temp_rule.dst=0;
    temp_rule.by.push_back('A');
    nfa.rules[0].push_back(temp_rule);
    std::vector<matchStr> matchallIndex=matchAllIndex(text);//matchIndex(text,0);
    if(matchallIndex.size()>0)//如果匹配成功
    {
        std::vector<std::vector<std::string>> matchall;
        for(int i=0;i<matchallIndex.size();i++)
        {
            matchall.push_back(matchallIndex[i].matchstr);
        }
        return matchall;
    }
    return {};
}

/**
 * 在给定的输入文本上，进行基于正则表达式的替换，返回替换完成的结果。
 * 需要支持分组替换，如$1表示将此处填入第一个分组匹配到的内容。具体的规则和例子详见文档。
 * @param text 输入的文本
 * @param replacement 要将每一处正则表达式的匹配结果替换为什么内容
 * @return 替换后的文本
 */
std::string Regex::replaceAll(std::string text, std::string replacement) {
    // TODO 请你完成这个函数
    //     当你在替换串 replacement 中发现形如 $ 后面跟着若干个数字的表达，如 $2 ，则实际此处替换的
    // 内容应为当前位置匹配结果的第2分组。
    // 例如，对输入串 ab12ab34 、pattern ab(\d+) 和替换串 c$1d ，替换的结果应为
    // abc12dabc34d ，直接暴力替换为 abc$1dabc$1d 是错误的结果。
    // 注意，分组序号可能不只有一位数。 $99 这种也是允许的，表示替换为第99分组。
    // 若要替换的分组在pattern中根本不存在，则约定替换为空串。例如对上例，pattern根本没有
    // 第二分组，则使用替换串 c$2d 替换的结果应为 abcdabcd 。
    // 由于允许了 $1 这种替换分组，因此需要允许 $ 字符转义，也就是当你发现连续的两个 $$ 时，实际
    // 替换的过程中作为一个 $ 处理。
    InsertInitial(nfa);
    Rule temp_rule;
    temp_rule.type=SPECIAL;
    temp_rule.dst=0;
    temp_rule.by.push_back('A');
    nfa.rules[0].push_back(temp_rule);
    std::vector<matchStr> matchallIndex=matchAllIndex(text);//匹配所有的结果
    if(matchallIndex.size()>0)//如果匹配成功
    {
        std::string result;
        int last=0;
        for(int i=0;i<matchallIndex.size();i++)//对每一个匹配结果进行替换
        {
            result+=text.substr(last,matchallIndex[i].init-last);
            last=matchallIndex[i].end;
            int j=0;
            while(j<replacement.size())
            {
                if(replacement[j]=='$')//如果是$，则进行替换
                {
                    if(j+1<replacement.size()&&replacement[j+1]=='$')//如果是$$，则替换为$
                    {
                        result+='$';
                        j+=2;
                    }
                    else//否则替换为分组
                    {
                        int k=j+1;
                        int num=0;
                        while(k<replacement.size()&&replacement[k]>='0'&&replacement[k]<='9')
                        {
                            num=num*10+replacement[k]-'0';
                            k++;
                        }
                        if(num<matchallIndex[i].matchstr.size())
                        {
                            result+=matchallIndex[i].matchstr[num];
                        }
                        j=k;
                    }
                }
                else
                {
                    result+=replacement[j];
                    j++;
                }
            }
        }
        result+=text.substr(last);
        return result;
    }
    return text;
}


/**
 * 解析正则表达式的字符串，生成语法分析树。
 * 你应该在compile函数中调用一次本函数，以得到语法分析树。
 * 通常，你不需要改动此函数，也不需要理解此函数实现每一行的具体含义。
 * 但是，你应当对语法分析树的数据结构(RegexContext)有一定的理解，作业文档中有相关的教程可供参考。
 * @param pattern 要解析的正则表达式的字符串
 * @return RegexContext类的对象的指针。保证不为空指针。
 */
regexParser::RegexContext *Regex::parse(const std::string &pattern) {
    if (antlrInputStream) throw std::runtime_error("此Regex对象已被调用过一次parse函数，不可以再次调用！");
    antlrInputStream = new antlr4::ANTLRInputStream(pattern);
    antlrLexer = new regexLexer(antlrInputStream);
    antlrTokenStream = new antlr4::CommonTokenStream(antlrLexer);
    antlrParser = new regexParser(antlrTokenStream);
    regexParser::RegexContext *tree = antlrParser->regex();
    if (!tree) throw std::runtime_error("parser解析失败(函数返回了nullptr)");
    auto errCount = antlrParser->getNumberOfSyntaxErrors();
    if (errCount > 0) throw std::runtime_error("parser解析失败，表达式中有" + std::to_string(errCount) + "个语法错误！");
    if (antlrTokenStream->LA(1) != antlr4::Token::EOF)
        throw std::runtime_error("parser解析失败，解析过程未能到达字符串结尾，可能是由于表达式中间有无法解析的内容！已解析的部分："
                                 + antlrTokenStream->getText(antlrTokenStream->get(0),
                                                             antlrTokenStream->get(antlrTokenStream->index() - 1)));
    return tree;
}

// 此析构函数是为了管理ANTLR语法分析树所使用的内存的。你不需要阅读和理解它。
Regex::~Regex() {
    delete antlrInputStream;
    delete antlrLexer;
    delete antlrTokenStream;
    delete antlrParser;
}
