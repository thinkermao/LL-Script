#ifndef __QUAD_GENERATOR_H__
#define __QUAD_GENERATOR_H__

#include <map>
#include <list>

#include "Quad.h"

namespace script
{
    // 
    // a help class
    //
    class QuadGenerator
    {
    public:
        QuadGenerator(std::list<Quad*> &codes) : codes_(codes) {}

        void insertCall(Label *begin, Value *result, int num);
        void insertInvoke(Value *function, Value *result, int num);
        void insertParam(Value *value);
        void insertSingle(unsigned op, Value *source, Value *result);
        void insertOperation(unsigned op, Value *left, Value *right, Value *result);
        void insertIf(Value *condition, Label *label);
        void insertIfFalse(Value *condition, Label *label);
        void insertStore(Value *id, Value *result);
        void insertLoad(Value *id, Value *result);
        void insertReturn(Value *value);
        void insertLabel(Label *label);
        void insertGoto(Label *label);
        void insertCopy(Value *source, Value *dest);

    private:
        std::list<Quad*> &codes_;
    };

    class QuadCode
    {
        friend class DumpQuad;
    public:
        QuadCode() : gen_(codes_) {}
        QuadGenerator *getGenerator() { return &gen_; }

    private:
        std::list<Quad*> codes_;

        QuadGenerator gen_;
    };

    class QuadFunction : public QuadCode
    {
        friend class DumpQuad;
    public:
        QuadFunction(std::string name, Label *begin, Label *end)
            : name_(std::move(name)), begin_(begin), end_(end)
        {}

        std::string &getName() { return name_; }

    private:
        std::string name_;
        Label *begin_;
        Label *end_;
    };

    class QuadModule : public QuadCode
    {
        friend class DumpQuad;
    public:
        ~QuadModule() { destory(); }

        QuadFunction *createFunction(std::string name, Label *begin, Label *end);
        QuadFunction *getFunction(std::string &name);

    private:
        void destory()
        {
            for (auto &i : functions_)
            {
                delete i.second;
                i.second = nullptr;
            }
        }

    private:
        std::map<std::string, QuadFunction*> functions_;
    };
}

#endif // !__QUAD_GENERATOR_H__
