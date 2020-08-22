#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include <iostream>
using namespace std;
using namespace clang;
using namespace oclint;

class QTTMethodParamRule : public AbstractASTVisitorRule<QTTMethodParamRule>
{
private:
    int _threshold;
    string _teamname;
    
public:
    virtual const string name() const override
    {
        return "方法格式规范";
    }
    
    virtual int priority() const override
    {
        return 3;
    }
    
    virtual const string category() const override
    {
        return _teamname + "方法格式";
    }

#ifdef DOCGEN
    virtual const std::string since() const override
    {
        return "0.13.1";
    }

    virtual const std::string description() const override
    {
        return ""; // TODO: fill in the description of the rule.
    }

    virtual const std::string example() const override
    {
        return R"rst(
.. code-block:: cpp

    void example()
    {
        // TODO: modify the example for this rule.
    }
        )rst";
    }

    /* fill in the file name only when it does not match the rule identifier
    virtual const std::string fileName() const override
    {
        return "";
    }
    */

    /* add each threshold's key, description, and its default value to the map.
    virtual const std::map<std::string, std::string> thresholds() const override
    {
        std::map<std::string, std::string> thresholdMapping;
        return thresholdMapping;
    }
    */

    /* add additional document for the rule, like references, notes, etc.
    virtual const std::string additionalDocument() const override
    {
        return "";
    }
    */

    /* uncomment this method when the rule is enabled to be suppressed.
    virtual bool enableSuppress() const override
    {
        return true;
    }
    */
#endif

    virtual void setUp() override {
        _teamname = RuleConfiguration::stringForKey("TEAM_NAME", "趣头条");
        _threshold = RuleConfiguration::intForKey("METHOD_PARAM", 3);
    }
    virtual void tearDown() override {}
    
    static vector<string> split(const string &str, const string &pattern)
    {
        char * strc = new char[strlen(str.c_str())+1];
        strcpy(strc, str.c_str());   //string转换成C-string
        vector<string> res;
        char* temp = strtok(strc, pattern.c_str());
        while(temp != NULL)
        {
            res.push_back(string(temp));
            temp = strtok(NULL, pattern.c_str());
        }
        delete[] strc;
        return res;
    }
    
    static string formatObjcMethodName(string name) {
        replace(name.begin(),name.end(),'\r',' ');
        replace(name.begin(),name.end(),'\n',' ');
        replace(name.begin(),name.end(),'\t',' ');
        vector<string> vec = split(name,":");
        vector<string> lineVec;
        string lPart,rPart;
        size_t nMaxDotPos = 0,pos;
        for(int i=0;i<vec.size();i++){
            string str = vec.at(i);
            trim(str);
            if(i==0){
                lPart = str;
                continue;
            }
            string tmpStr;
            pos = str.find(")");
            rPart=str.substr(0,pos+1);
            tmpStr = str.substr(pos+1,str.length()-pos-1);
            trim(tmpStr);
            pos = tmpStr.find(" ");
            if(pos != string::npos){
                rPart+=tmpStr.substr(0,pos+1);
                lineVec.push_back(trim(lPart)+":"+trim(rPart));
                nMaxDotPos = (lPart.length()>nMaxDotPos?lPart.length():nMaxDotPos);
                lPart = tmpStr.substr(pos+1,tmpStr.length()-pos-1);
            }
            else{
                rPart+=tmpStr;
                lineVec.push_back(trim(lPart)+":"+trim(rPart));
            }
        }
        if(!lineVec.size() && lPart.length()){
            lineVec.push_back(trim(lPart));
        }
        string joinedStr;
        if(lineVec.size()==1){
            joinedStr = lineVec.at(0);
        }
        else{
            for(size_t i=0;i!=lineVec.size();i++){
                string line = lineVec.at(i);
                pos = line.find(":");
                string tmpStr(i==0?0:nMaxDotPos-pos,' ');
                joinedStr+=tmpStr+line;
                if(i!=lineVec.size()-1)
                    joinedStr+="\n";
            }
        }
        return joinedStr;
    }
    
    static inline string &ltrim(std::string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(),
                                   not1(ptr_fun<int, int>(isspace))));
        return s;
    }
    
    // trim from end
    static inline string &rtrim(string &s) {
        s.erase(find_if(s.rbegin(), s.rend(),
                        not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
    }
    
    // trim from both ends
    static inline string &trim(string &s) {
        return ltrim(rtrim(s));
    }
    
    bool VisitObjCMethodDecl(ObjCMethodDecl *node)
    {
        string name = node->getNameAsString();
        if( node->param_size()>_threshold){
            string methodDeclStr;
            ASTContext *context = _carrier->getASTContext();
            SourceLocation begin = node->getSourceRange().getBegin();
            SourceLocation end = node->getSourceRange().getEnd(); methodDeclStr.assign(context->getSourceManager().getCharacterData(begin),end.getRawEncoding()-node->getSourceRange().getBegin().getRawEncoding());
            string replaceName = formatObjcMethodName(methodDeclStr);
            if(methodDeclStr.find(replaceName)){
                addViolation(node, this, "方法参数超过"+to_string(_threshold)+"个，需要折行并冒号对齐");
            }
        }
        return true;
    }

};

static RuleSet rules(new QTTMethodParamRule());
