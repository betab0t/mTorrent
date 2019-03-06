#include "bencode.h"

size_t BInt::size()
{
    return count_digits(this->raw()) + 2;
}

size_t BString::size()
{
    size_t len = this->raw().length();
    return len + count_digits(len) + 1;
}

size_t BList::size()
{
    size_t total = 2;
    for(const auto& item : this->raw())
    {
        total += item->size();
    }
    return total;
}

size_t BDict::size()
{
    size_t total = 2;
    for (const auto& item : this->raw())
    {
        total += item.first->size();
        total += item.second->size();
    }
    return total;
}

std::unique_ptr<BInt> Bencode::parse_int(std::string& buf)
{
    try
    {
        return std::make_unique<BInt>(std::stoi(buf.substr(1)));
    }
    catch(const std::invalid_argument& e)
    {
        throw FormatError("Invalid integer format!");
    }
}

std::unique_ptr<BString> Bencode::parse_string(std::string& buf)
{
    size_t len, colon_pos;
    try
    {
        len = stoi(buf);
    }
    catch(const std::invalid_argument& e)
    {
        throw FormatError("Invalid string length!");
    }

    colon_pos = count_digits(len);
    if (buf[colon_pos] != ':' || colon_pos + len > buf.size() - 1)
    {
        throw FormatError("Invalid string format");
    }
    std::string data = buf.substr(++colon_pos, len);

    return std::make_unique<BString>(data);
}

std::unique_ptr<BList> Bencode::parse_list(std::string& buf)
{
    std::unique_ptr<BList> blist = std::make_unique<BList>();
    std::unique_ptr<BItem> bitem = NULL;
    size_t pos = 1;
    std::string temp;

    while(buf[pos] != 'e')
    {
        temp = buf.substr(pos);
        bitem = this->decode(temp);
        pos += bitem->size();
        blist->raw().push_back(std::move(bitem));
    }
    return blist;
}

std::unique_ptr<BDict> Bencode::parse_dict(std::string& buf)
{
    std::unique_ptr<BDict> bdict = std::make_unique<BDict>();
    std::unique_ptr<BString> key = NULL;
    std::unique_ptr<BItem> val = NULL;
    size_t pos = 1;
    std::string temp;

    while(buf[pos] != 'e')
    {
        // decode key
        temp = buf.substr(pos);
        key = this->parse_string(temp);
        pos += key->size();

        // decode value
        temp = buf.substr(pos);
        val = this->decode(temp);
        pos += val->size();

        // insert
        bdict->raw().insert(std::make_pair(std::move(key), std::move(val)));
    }
    return bdict;
}

std::unique_ptr<BItem> Bencode::decode(std::string& buf)
{
    switch (buf[0])
    {
        case 'i':
            return this->parse_int(buf);
            
        case 'l':
            return this->parse_list(buf);
        
        case 'd':
            {
                return this->parse_dict(buf);
            }
        default:
            if (0 <= buf[0] <= '9')
                return this->parse_string(buf);
            throw FormatError("Unknown type!");
    }
}

std::string Bencode::to_string(BItem* bitem)
{
    std::string out;
    auto bint = dynamic_cast<BInt*>(bitem);
    if (bint)
    {
        out = std::to_string(bint->raw());
    }

    auto bstring = dynamic_cast<BString*>(bitem);
    if (bstring)
    {
        out = "'" + bstring->raw() + "'";
    }

    auto blist = dynamic_cast<BList*>(bitem);
    if (blist)
    {
        out = "[";
        for (auto const& item : blist->raw())
        {
            if(out.back() != '[')
                out += ", "; // add sep starting 2nd iter
            out += this->to_string(item.get());
        }
        out += "]";
    }

    auto bdict = dynamic_cast<BDict*>(bitem);
    if (bdict)
    {
        out = "{";
        for (auto const& item : bdict->raw())
        {
            if(out.back() != '{')
                out += ", "; // add sep starting 2nd iter
            out += this->to_string(item.first.get()) + ": " + this->to_string(item.second.get());
        }
        out += "}";
    }
    return out;
}