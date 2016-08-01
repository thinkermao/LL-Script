#pragma once

#include <list>
#include <string>
#include <vector>

namespace script
{

    class BasicBlock;

namespace ir 
{
    class Value;
    class User;

    class Use
    {
    public:
        Use(Value *value, User *user);
        ~Use();

        Value *getValue() const { return value_; }
        User *getUser() const { return user_; }

    private:
        Value *value_;
        User *user_;
        User *prev_, *next_;
    };

    class Value
    {
    public:
        virtual ~Value() = default;

        // addUse/killUse - These two methods should only used by the Use class.
        void addUse(Use &u) { uses_.push_back(u); }
        void killUse(Use &u) { uses_.remove(u); }
        User *use_back() { return uses_.back().getUser(); }
    
    protected:
        std::list<Use> uses_;
        std::string name_;
    };

    class Constant : public Value
    {
    public:
        enum { Character, Integer, Float, String, };

        Constant(int num);
        Constant(char c);
        Constant(float fnum);
        Constant(std::string str);

        virtual ~Constant() = default;

        unsigned type() const { return type_; }
        int getInteger() const { return num_; }
        char getChar() const { return c_; }
        float getFloat() const { return fnum_; }
        const std::string &getString() const { return str_; }

    protected:
        unsigned type_;
        int num_;
        char c_;
        float fnum_;
        std::string str_;
    };

    class User : public Value 
    {
    protected:
        // ���Ӧ�ģ�Userʹ��vector����֯Use Object
        std::vector<Use> operands_;
    public:
        virtual ~User() = default;
        typedef std::vector<Use>::iterator op_iterator;
        unsigned getNumOperands() const { return operands_.size(); }
        void op_reserve(unsigned NumElements) { operands_.reserve(NumElements); }
        op_iterator op_begin() { return operands_.begin(); }
        op_iterator op_end() { return operands_.end(); }
    };

    enum class Instructions
    {
        IR_Alloca,
        IR_Load,
        IR_Store,
        IR_Invoke,
        IR_Branch,
        IR_Assign,
        IR_NotOp,
        IR_Return,
        IR_ReturnVoid,
        IR_BinaryOps,
        IR_Index,
        IR_SetIndex,
    };

    enum class BinaryOps
    {
        BO_Add,
        BO_Sub,
        BO_Mul,
        BO_Div,
        BO_And,
        BO_Or,
        BO_Great,
        BO_NotLess,
        BO_Less,
        BO_NotGreat,
        BO_Equal,
        BO_NotEqual,
    };

    class Instruction : public User 
    {
    public:
        virtual ~Instruction() = default;
        virtual Instructions instance() const = 0;

        Instruction(const std::string &name, Instruction *before);
        Instruction(const std::string &name, BasicBlock *end);

    protected:    
        BasicBlock *parent_;
        Instruction *prev_, *next_;
        // ...

        std::string name_;
    };

    class Alloca : public Instruction
    {
    public:
        Alloca(const std::string &name, Instruction *insertBefore)
            : Instruction(name, insertBefore)
        { }

        Alloca(const std::string &name, BasicBlock *insertAtEnd)
            : Instruction(name, insertAtEnd)
        { }

        virtual ~Alloca() = default;
        virtual Instructions instance() const { return Instructions::IR_Alloca; }
    };

    class Load : public Instruction
    {
    public:
        Load(Value *from, const std::string &name, Instruction *insertBefore)
            : Instruction(name, insertBefore)
        {
            init(from);
        }

        Load(Value *from, const std::string &name, Instruction *insertAtEnd)
            : Instruction(name, insertAtEnd)
        { 
            init(from);
        }

        virtual ~Load() = default;
        virtual Instructions instance() const { return Instructions::IR_Load; }

    protected:
        void init(Value *from);
    };

    class Store : public Instruction
    {
    public:
        Store(Value *value, Value *addr, Instruction *insertBefore)
            : Instruction("", insertBefore)
        { 
            init(value, addr);
        }

        Store(Value *value, Value *addr, BasicBlock *insertAtEnd)
            : Instruction("", insertAtEnd)
        { 
            init(value, addr);
        }

        virtual ~Store() = default;
        virtual Instructions instance() const { return Instructions::IR_Store; }

    protected:
        void init(Value *value, Value *addr);
    };

    class Invoke : public Instruction
    {
    public:
        Invoke(const std::string functionName, const std::vector<Value*> &args,
            const std::string name, Instruction *insertBefore)
            : Instruction(name, insertBefore), functionName_(functionName)
        {
            init(functionName, args);
        }

        Invoke(const std::string functionName, const std::vector<Value*> &args,
            const std::string name, BasicBlock *insertAtEnd)
            : Instruction(name, insertAtEnd), functionName_(functionName)
        {
            init(functionName, args);
        }

        virtual ~Invoke() = default;
        virtual Instructions instance() const { return Instructions::IR_Invoke; }

    protected:
        void init(const std::string functionName, const std::vector<Value*> &args);

    protected:
        std::string functionName_;
    };

    class Branch : public Instruction
    {
    public:
        Branch(Value *cond, BasicBlock *then, BasicBlock *_else,
            Instruction *insertBefore)
            : Instruction("", insertBefore), then_(then), else_(_else)
        {
            init(cond);
        }

        Branch(Value *cond, BasicBlock *then, BasicBlock *_else,
            BasicBlock *insertAtEnd)
            : Instruction("", insertAtEnd), then_(then), else_(_else)
        {
            init(cond);
        }

        Branch(Value *cond, BasicBlock *then, Instruction *insertBefore)
            : Instruction("", insertBefore), then_(then), else_(nullptr)
        {
            init(cond);
        }

        Branch(Value *cond, BasicBlock *then, BasicBlock *insertAtEnd)
            : Instruction("", insertAtEnd), then_(then), else_(nullptr)
        {
            init(cond);
        }

        virtual ~Branch() = default;
        virtual Instructions instance() const { return Instructions::IR_Branch; }

        bool hasElseBlock() const { return else_ != nullptr; }

    protected:
        void init(Value *cond);

    protected:
        BasicBlock *then_;
        BasicBlock *else_;
    };

    // TODO: ��������д�ȷ���Ƿ���Ҫ
    class Assign : public Instruction
    {
    public:
    };

    class NotOp : public Instruction
    {
    public:
        NotOp(Value *value, const std::string &name, Instruction *insertBefore)
            : Instruction(name, insertBefore)
        {
            init(value);
        }

        NotOp(Value *value, const std::string &name, BasicBlock *insertAtEnd)
            : Instruction(name, insertAtEnd)
        {
            init(value);
        }

        virtual ~NotOp() = default;
        virtual Instructions instance() const { return Instructions::IR_NotOp; }

    protected:
        void init(Value *value);
    };

    class Return : public Instruction
    {
    public:
        Return(Value *value, Instruction *insertBefore)
            : Instruction("", insertBefore)
        {
            init(value);
        }

        Return(Value *value, BasicBlock *insertAtEnd)
            : Instruction("", insertAtEnd)
        {
            init(value);
        }

        virtual ~Return() = default;
        virtual Instructions instance() const { return Instructions::IR_Return; }

    protected:
        void init(Value *value);
    };

    class ReturnVoid : public Instruction
    {
    public:
        ReturnVoid(Instruction *insertBefore)
            : Instruction("", insertBefore)
        {}

        ReturnVoid(BasicBlock *insertAtEnd)
            : Instruction("", insertAtEnd)
        {}

        virtual ~ReturnVoid() = default;
        virtual Instructions instance() const { return Instructions::IR_ReturnVoid; }
    };

    class BinaryOperator : public Instruction 
    {
    public:
        BinaryOperator(BinaryOps bop, Value *lhs, Value *rhs,
            const std::string &name, Instruction *insertBefore)
            : Instruction(name, insertBefore) 
        {
            init(bop, lhs, rhs);
        }
        BinaryOperator(BinaryOps bop, Value *lhs, Value *rhs,
            const std::string &name, BasicBlock *insertAtEnd)
            : Instruction(name, insertAtEnd) 
        {
            init(bop, lhs, rhs);
        }
        
        virtual ~BinaryOperator() = default;
        virtual Instructions instance() const { return Instructions::IR_BinaryOps; }

    protected:
        void init(BinaryOps bop, Value *lhs, Value* rhs);
    };

    class Index : public Instruction
    {
    public:
        Index(Value *table, Value *index, const std::string &name, 
            Instruction *insertBefore)
            : Instruction(name, insertBefore)
        {
            init(table, index);
        }

        Index(Value *table, Value *index, const std::string &name,
            Instruction *insertAtEnd)
            : Instruction(name, insertAtEnd)
        {
            init(table, index);
        }

        virtual ~Index() = default;
        virtual Instructions instance() const { return Instructions::IR_Index; }

    protected:
        void init(Value *table, Value *index);
    };

    class SetIndex : public Instruction
    {
    public:
        SetIndex(Value *table, Value *index, Value *to, Instruction *insertBefore)
            : Instruction("", insertBefore)
        {
            init(table, index, to);
        }

        SetIndex(Value *table, Value *index, Value *to, BasicBlock *insertAtEnd)
            : Instruction("", insertAtEnd)
        {
            init(table, index, to);
        }

        virtual ~SetIndex() = default;
        virtual Instructions instance() const { return Instructions::IR_SetIndex; }

    protected:
        void init(Value *table, Value *index, Value *to);
    };

}
}