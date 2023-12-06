#include "val.h"

Value Value::operator+(const Value& op) const {
    Value val;
    if (!((op.IsReal() || op.IsInt()) && (IsInt() || IsReal()))) {
        return val;
    }
    if (GetType() == op.GetType()) {
        if (IsReal()) {
            val = GetReal() + op.GetReal();
        } else {
            val = GetInt() + op.GetInt();
        }
    } else {
        if (IsReal()) {
            // first operator is real, second is int
            val = GetReal() + op.GetInt();
        } else {
            // first operator is int, second is real
            val = GetInt() + op.GetReal();
        }
    }
    return val;
}

Value Value::operator-(const Value& op) const {
    Value val;
    if (!((op.IsReal() || op.IsInt()) && (IsInt() || IsReal()))) {
        return val;
    }
    if (GetType() == op.GetType()) {
        if (IsReal()) {
            val = GetReal() - op.GetReal();
        } else {
            val = GetInt() - op.GetInt();
        }
    } else {
        if (IsReal()) {
            // first operator is real, second is int
            val = GetReal() - op.GetInt();
        } else {
            // first operator is int, second is real
            val = GetInt() - op.GetReal();
        }
    }
    return val;
}

Value Value::operator*(const Value& op) const {
    Value val;
    if (!((op.IsReal() || op.IsInt()) && (IsInt() || IsReal()))) {
        return val;
    }
    if (GetType() == op.GetType()) {
        if (IsReal()) {
            val = GetReal() * op.GetReal();
        } else {
            val = GetInt() * op.GetInt();
        }
    } else {
        if (IsReal()) {
            // first operator is real, second is int
            val = GetReal() * op.GetInt();
        } else {
            // first operator is int, second is real
            val = GetInt() * op.GetReal();
        }
    }
    return val;
}

Value Value::operator/(const Value& op) const {
    Value val;
    if (!((op.IsReal() || op.IsInt()) && (IsInt() || IsReal()))) {
        return val;
    }
    if (GetType() == op.GetType()) {
        if (IsReal()) {
            val = GetReal() / op.GetReal();
        } else {
            val = GetInt() / op.GetInt();
        }
    } else {
        if (IsReal()) {
            // first operator is real, second is int
            val = GetReal() / op.GetInt();
        } else {
            // first operator is int, second is real
            val = GetInt() / op.GetReal();
        }
    }
    return val;
    
}

Value Value::div(const Value& op) const {
    Value val;
    if (IsInt() && op.IsInt()) {
        val = GetInt() / op.GetInt();
    }
    return val;
    
}

Value Value::idiv(const Value& op) const {
    Value val;
    if (IsInt() && op.IsInt()) {
        val = GetInt() / op.GetInt();
    }
    return val;
    
}

Value Value::operator==(const Value& op) const {
    //both operands need to be the same type unless int and real
    Value val;
    if (GetType() == op.GetType() || (IsReal() && op.IsInt()) || (IsInt() && op.IsReal())) {
        if (IsReal()) {
            if (op.IsReal()) {
                val = (GetReal() == op.GetReal());
            } else {
                val = (GetReal() == op.GetInt());
            }
        } else if (IsInt()) {
            if (op.IsReal()) {
                val = (GetInt() == op.GetReal());
            } else {
                val = (GetInt() == op.GetInt());
            }
        } else if (IsBool()) {
            val = (GetBool() == op.GetBool());
        } else if (IsString()) {
            val = (GetString() == op.GetString());
        }
    } 
    return val;
}

Value Value::operator>(const Value& op) const {
    //both operands need to be the same type unless int and real
    Value val;
    if (GetType() == op.GetType() || (IsReal() && op.IsInt()) || (IsInt() && op.IsReal())) {
        if (IsReal()) {
            if (op.IsReal()) {
                val = (GetReal() > op.GetReal());
            } else {
                val = (GetReal() > op.GetInt());
            }
        } else if (IsInt()) {
            if (op.IsReal()) {
                val = (GetInt() > op.GetReal());
            } else {
                val = (GetInt() > op.GetInt());
            }
        } else if (IsBool()) {
            val = (GetBool() > op.GetBool());
        } else if (IsString()) {
            val = (GetString() > op.GetString());
        }
    } 
    return val;
}

Value Value::operator<(const Value& op) const {
    //both operands need to be the same type unless int and real
    Value val;
    if (GetType() == op.GetType() || (IsReal() && op.IsInt()) || (IsInt() && op.IsReal())) {
        if (IsReal()) {
            if (op.IsReal()) {
                val = (GetReal() < op.GetReal());
            } else {
                val = (GetReal() < op.GetInt());
            }
        } else if (IsInt()) {
            if (op.IsReal()) {
                val = (GetInt() < op.GetReal());
            } else {
                val = (GetInt() < op.GetInt());
            }
        } else if (IsBool()) {
            val = (GetBool() < op.GetBool());
        } else if (IsString()) {
            val = (GetString() < op.GetString());
        }
    } 
    return val;
}

Value Value::operator&&(const Value& oper) const {
    Value val;
    if (IsBool() && oper.IsBool()) {
        val = (GetBool() && oper.GetBool());
    }
    
    return val;
}

Value Value::operator||(const Value& oper) const {
    Value val;
    if (IsBool() && oper.IsBool()) {
        val = (GetBool() || oper.GetBool());
    }
    
    return val;
}

Value Value::operator!() const {
    Value val;
    if (IsBool()) {
        val = !GetBool();
    }
    return val;
}

Value Value::operator%(const Value& oper) const {
    Value val;
    if (IsInt() && oper.IsInt()) {
        val = (GetInt() % oper.GetInt());
    }
    return val;
}