#pragma once

#include <memory>
#include <vector>
#include <map>
#include <cmath>

#define count_digits(x) (floor(log10((x))) + 1)

class BItem
{
public:
    virtual size_t size() { return 0; };
};

class BInt : public BItem
{
private:
    int val;
public:
    BInt(int val) : val(val){};
    int raw() { return this->val; };
    size_t size() override;
};

class BString : public BItem
{
private:
    std::string str;
public:
    BString(std::string& str) : str(str){};
    std::string raw() { return this->str; };
    size_t size() override;
};

class BList : public BItem
{
private:
    std::vector<std::unique_ptr<BItem>> list;
public:
    std::vector<std::unique_ptr<BItem>>& raw() { return this->list; };
    size_t size() override;
};

class BDict : public BItem
{
private:
    std::map<std::unique_ptr<BString>, std::unique_ptr<BItem>> dict;

public:
    std::map<std::unique_ptr<BString>, std::unique_ptr<BItem>>& raw() { return this->dict; };
    size_t size() override;
};

class Bencode
{
private:
    std::unique_ptr<BInt> parse_int(std::string& buf);
    std::unique_ptr<BString> parse_string(std::string& buf);
    std::unique_ptr<BList> parse_list(std::string& buf);
    std::unique_ptr<BDict> parse_dict(std::string& buf);
    
public:
    std::unique_ptr<BItem> decode(std::string& buf);
    std::string to_string(BItem* bitem);
};

class FormatError : public std::exception
{
private:
    std::string why;

public:
    FormatError(const char* why):
        why(why) {};

    virtual const char* what() const throw()
    {
        return why.c_str();
    }
};