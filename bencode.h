#pragma once

#include <memory>
#include <vector>
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
    // use a vector(not map/unorderd_map) to save the original insertion order
    std::vector<std::pair<std::unique_ptr<BString>, std::unique_ptr<BItem>>> dict;

public:
    std::vector<std::pair<std::unique_ptr<BString>, std::unique_ptr<BItem>>>& raw() { return this->dict; };
    size_t size() override;
    BItem* find(std::string key);
};

class Bencode
{
private:
    std::unique_ptr<BInt> decodeInt(std::string& buf);
    std::unique_ptr<BString> decodeStr(std::string& buf);
    std::unique_ptr<BList> decodeList(std::string& buf);
    std::unique_ptr<BDict> decodeDict(std::string& buf);
    
    std::string encodeInt(BInt* i);
    std::string encodeStr(BString* s);
    std::string encodeList(BList* l);
    std::string encodeDict(BDict* d);

public:
    std::unique_ptr<BItem> decode(std::string& buf);
    std::string encode(BItem* item);
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