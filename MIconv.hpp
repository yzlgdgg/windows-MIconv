#pragma once
#ifndef MICONV_HPP
#define MICONV_HPP

#include<iostream>
#include<memory>
#include<string>

#include<windows.h>

namespace MIconv{
    using namespace std;

    class MUnicode;
    class MACP;
    class MUTF8;

    template<class T>
    T MIconv_cast(MACP acp);

    template<class T>
    T MIconv_cast(MUnicode unicode);

    void delete_array(wchar_t* p){
        delete[] p;
    }
    //Unicode编码类型,自带size确认大小
    class MUnicode{
    public:
        MUnicode(){
            wstr.reset();
            size = 0;
        }
        MUnicode(const wchar_t* ws, size_t _size){
            set(ws, _size);
        }
        MUnicode(const MUnicode& Unicode){
            wstr = Unicode.wstr;
            size = Unicode.size;
        }
        template<class T>
        friend T MIconv_cast(MUnicode unicode){
            T res;
            res.ToCur(unicode);
            return res;
        }
        wchar_t* get() const{
            if(wstr){
                return wstr.get();
            }
            return nullptr;
        }
        size_t getsize() const{
            return size;
        }
        void set(const wchar_t* ws, size_t _size){
            if(_size > 0){
                wstr.reset(new wchar_t[_size + 1], delete_array);
                memset(wstr.get(), 0, _size + 1);
                memcpy(wstr.get(), ws, _size);
            }
            size = _size;
        }
        void resize(size_t _size){
            if(_size > 0){
                wstr.reset(new wchar_t[_size + 1], delete_array);
                memset(wstr.get(), 0, _size + 1);
            }
            size = _size;
        }
        void clear(){
            wstr.reset();
            size = 0;
        }
    private:
        shared_ptr<wchar_t> wstr;
        size_t size;
    };

    //ACP本机编码
    class MACP{
        
    public:
        MACP(){
            setcodepage(CP_ACP);
        }
        MACP(const MACP& acp){
            str = acp.str;
            CodePage = acp.CodePage;
        }
        MACP(const string &s){
            if(s.size() > 0){
                str = s;
            }
            setcodepage(CP_ACP);
        }
        MACP(const char* s){
            if(s != nullptr){
                str = s;
            }
            setcodepage(CP_ACP);
        }
        ~MACP(){}

        friend ostream& operator<<(ostream& out, const MACP& m){
            out << m.str;
            return out;
        }
        operator string(){
            return str;
        }
        operator bool(){
            return str.empty();
        }
        string get() const{
            return str;
        }
        size_t getsize() const{
            return str.size();
        }
        void setcodepage(unsigned int cp){
            CodePage = cp;
        }
        void set(const char* s){
            str = s;
        }
        void set(const string& s){
            str = s;
        }
        template<class T>
        friend T MIconv_cast(MACP acp){
            MUnicode Unicode = acp.ToUnicode();
            T res;
            res.ToCur(Unicode);
            return res;
        }
        MUnicode ToUnicode(){
            MUnicode unicode;
            if(str.size()){
                size_t size = MultiByteToWideChar(CodePage, 0, str.c_str(), str.size(), nullptr, 0);
                unicode.resize(size);
                MultiByteToWideChar(CodePage, 0, str.c_str(), str.size(), unicode.get(), size);
            }
            return unicode;
        }
        void ToCur(const MUnicode& unicode){
            if(unicode.getsize() > 0){
                size_t size = WideCharToMultiByte(CodePage, 0, unicode.get(), unicode.getsize(), nullptr, 0, nullptr, nullptr);
                str.resize(size+1);
                WideCharToMultiByte(CodePage, 0, unicode.get(), unicode.getsize(), &str[0], size, nullptr, nullptr);
                str[size+1] = '\0';
            }
        }
    protected:
        string str;
        unsigned int CodePage;
    };

    //UTF8类型
    class MUTF8 : public MACP{
    public:
        MUTF8(){
            setcodepage(CP_UTF8);
        }
        MUTF8(const char* s) : MACP(s){
            setcodepage(CP_UTF8);
        }
        MUTF8(const string& s) : MACP(s){
            setcodepage(CP_UTF8);
        }
        ~MUTF8(){
        }
    };
}
#endif
