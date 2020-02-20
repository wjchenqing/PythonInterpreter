#include "Evalvisitor.h"


std::ostream &operator<< (const std::ostream &os,const Object &b) {
    os << b.toSTRING();
    return const_cast<ostream &>(os);
}

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
    string name = ctx->NAME()->toString();
    Function func(visit(ctx->parameters()).as<Function::Para>(), ctx->suite());
    Funcs.insert({name, func});
    return Object(NONE);
}
antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
    if (ctx->typedargslist())
        return visit(ctx->typedargslist());
    return Function::Para();
}
antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
    auto tfpdefs = ctx->tfpdef();
    auto tests = ctx->test();
    Function::Para parameter;
    int num1 = tfpdefs.size();
    int num2 = tests.size();
    int delta = num1- num2;
    for (int i = 0; i < num1; ++i)
        parameter.push_back({tfpdefs[i]->toString(),
                             (i < delta) ? Object() : visit(tests[i - delta]).as<Object>()});
    return parameter;
}
antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
    return visit(ctx->small_stmt());
}
antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
//    auto x = ctx->expr_stmt();
    if (ctx->expr_stmt() != nullptr) {
//        cout << ctx->expr_stmt() << endl;
        return visit(ctx->expr_stmt());
    } else if (ctx->flow_stmt())
        return visit(ctx->flow_stmt());
//    return Object(ATOMS);
}
antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
    auto testlists = ctx->testlist();
//    cout << ctx->augassign() << endl;
    if (ctx->augassign() == nullptr) {
        auto tmp = visit(testlists.back()).as<Object>();
        auto &cur = tmp.list_py;
        for (int i = 0; i < testlists.size() - 1; ++i)
            for (int j = 0; j < cur.size(); ++j )
                Assign_Var(testlists[i]->test()[j]->or_test()->and_test()[0]->not_test()[0]->comparison()->arith_expr()[0]->term()[0]->factor()[0]->atom_expr()->atom()->NAME()->toString(), cur[j]);
        return tmp;
    }
    Object name(testlists[0]->test()[0]->or_test()->and_test()[0]->not_test()[0]->comparison()->arith_expr()[0]->term()[0]->factor()[0]->atom_expr()->atom()->NAME()->toString());
    auto sign = ctx->augassign();
    Object& former = GetVar(name)->second;
    Object latter = visit(testlists[1]).as<Object>().list_py[0];
    if (sign->ADD_ASSIGN()) {
        former += latter;
        return former;
    }
    if (sign->SUB_ASSIGN()) {
        former -= latter;
        return former;
    }
    if (sign->MULT_ASSIGN()) {
        former *= latter;
        return former;
    }
    if (sign->DIV_ASSIGN()) {
        former /= latter;
        return former;
    }
    if (sign->MOD_ASSIGN()) {
        former %= latter;
        return former;
    }
    if (sign->IDIV_ASSIGN()) return former.intdiv(latter);
}
antlrcpp::Any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) {
    return Object(BREAK);
}
antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) {
    return Object(CONTINUE);
}
antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
    if (ctx->testlist() == nullptr) return Object(RETURN);
    Object cur = visit(ctx->testlist()).as<Object>();
    if (cur.list_py.size() == 1) {
        Object tmp = cur.list_py[0];
        tmp.flow_py = RETURN;
        return tmp;
    }
    cur.flow_py = RETURN;
    return cur;
}
antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
    auto tests = ctx->test();
    auto suites = ctx->suite();
    for (unsigned i = 0; i < tests.size(); ++i) {
        if (visit(tests[i]).as<Object>().toBOOL())
            return visit(suites[i]);
    }
    if (ctx->ELSE()) return visit(suites.back());
}
antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
    while (visit(ctx->test()).as<Object>().toBOOL()) {
        Object cur = visit(ctx->suite());
        if (cur.flow_py == BREAK) break;
        if (cur.flow_py == RETURN) return cur;
    }
    return Object();
}
antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
    if (ctx->simple_stmt())
        return visit(ctx->simple_stmt()).as<Object>();
    auto stmts = ctx->stmt();
    for (auto &itm: stmts) {
        Object tmp = visit(itm).as<Object>();
        if (tmp.flow_py != ATOMS) return tmp;
    }
    return Object();
}
antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
    return visit(ctx->or_test());
}
antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
    auto and_tests = ctx->and_test();
    if (ctx->OR().empty()) return visit(and_tests[0]);
    for (int i = 0; i <= ctx->OR().size(); ++i)
        if (visit(and_tests[i]).as<Object>().toBOOL()) return Object(true);
    return Object(false);
}
antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
    auto not_tests = ctx->not_test();
    if (!ctx->AND(0)) return visit(not_tests[0]);
    int num = 0;
    while (num < ctx->AND().size()) {
        if (!visit(not_tests[num]).as<Object>().toBOOL())
            return Object(false);
    }
    return Object(true);
}
antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
    if(ctx->NOT() && ctx->not_test()) return !visit(ctx->not_test()).as<Object>();
    return visit(ctx->comparison());
}
antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
    auto arith_exprs = ctx->arith_expr();
    auto comp_ops = ctx->comp_op();
    if (comp_ops.empty()) return visit(arith_exprs[0]);
    int num = 0;
    Object cur(true);
    Object pre = visit(arith_exprs[num]).as<Object>();
    for (auto i:comp_ops) {
        if (!cur.toBOOL()) return Object(false);
        Object latter = visit(arith_exprs[++num]).as<Object>();
        if (i->LESS_THAN()) {
            cur = pre < latter;
            pre = latter;
            continue;
        } else if (i->GREATER_THAN()) {
            cur = pre > latter;
            pre = latter;
            continue;
        } else if (i->EQUALS()) {
            cur = pre == latter;
            pre = latter;
        } else if (i->LT_EQ()) {
            cur = pre <= latter;
            pre = latter;
        } else if (i->GT_EQ()) {
            cur = pre >= latter;
            pre = latter;
        } else if (i->NOT_EQ_2()) {
            cur = pre != latter;
            pre = latter;
        }
    }
    return cur;
}
antlrcpp::Any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
    auto terms = ctx->term();
    auto addsub_ops = ctx->addsub_op();
    if (addsub_ops.empty()) return visit(terms[0]);
    int num = 0;
    auto cur = visit(terms[num]).as<Object>();
    for (auto i:addsub_ops){
        auto term = visit(terms[++num]).as<Object>();
        if (i->ADD())
            cur += term;
        else if (i->MINUS())
            cur -= term;
    }
    return cur;
}
antlrcpp::Any EvalVisitor::visitAddsub_op(Python3Parser::Addsub_opContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
    auto factors = ctx->factor();
    auto muldiv_ops = ctx->muldiv_op();
    if (muldiv_ops.empty()) return visit(factors[0]);
    int num = 0;
    Object cur = visit(factors[num]).as<Object>();
    for (auto i:muldiv_ops) {
        if (i->STAR()) cur *= visit(factors[++num]).as<Object>();
        else if (i->DIV()) cur /= visit(factors[++num]).as<Object>();
        else if (i->IDIV()) cur = cur.intdiv(visit(factors[++num]).as<Object>());
        else if (i->MOD()) cur %= visit(factors[++num]).as<Object>();
    }
    return cur;
}
antlrcpp::Any EvalVisitor::visitMuldiv_op(Python3Parser::Muldiv_opContext *ctx) {
    return visitChildren(ctx);
}
antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
    if (ctx->atom_expr()) return visit(ctx->atom_expr());
    if (ctx->addsub_op()->ADD()) return visit(ctx->factor());
    auto tmp = visit(ctx->factor()).as<Object>();
    return -tmp;
}
antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
    if (ctx->trailer() == nullptr) return visit(ctx->atom());
    std::string name = ctx->atom()->NAME()->toString();
    if (name == "print") {
        if (ctx->trailer()->arglist() == nullptr) {
            std::cout << '\n';
            return Object();
        }
        List tmp = visit(ctx->trailer()).as<Object>().list_py;
        int num = tmp.size() - 1;
        for (int i = 0; i <= num; ++i) {
            std::cout << tmp[i].toSTRING() << ((i == num) ? '\n' : ' ');
            std::cout.flush();
        }
        return Object(NONE);
    }
    if (name == "int")
        return Object(visit(ctx->trailer()).as<Object>().list_py[0].toINT());
    if (name == "float")
        return Object(visit(ctx->trailer()).as<Object>().list_py[0].toFLOAT());
    if (name == "bool")
        return Object(visit(ctx->trailer()).as<Object>().list_py[0].toBOOL());
    if (name == "str")
        return Object(visit(ctx->trailer()).as<Object>().list_py[0].toSTRING());
    auto tmp = Funcs.find(name);
    if (tmp == Funcs.end()) return Object(NONE);
    auto &function = tmp->second;
    Ob_Map o_tmp;
    if (ctx->trailer()->arglist()) {
        auto arguments = ctx->trailer()->arglist()->argument();
        int num = arguments.size();
        for (int i = 0; i < num; ++i) {
            if (arguments[i]->NAME() != nullptr)
                o_tmp.insert({arguments[i]->NAME()->toString(), visit(arguments[i]->test())});
            else
                o_tmp.insert({function.paras[i].first, visit(arguments[i]->test())});
        }
        for (auto &para: function.paras)
            if (o_tmp.find(para.first) == o_tmp.end()) o_tmp.insert(para);
        vars.push_back(o_tmp);
        Object cur = visit(function.suis).as<Object>();
        cur.flow_py = ATOMS;
        vars.pop_back();
        return cur;
    }

}
antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
    return visit(ctx->arglist());
}
antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
    if (ctx->NONE()) return Object(NONE);
    else if (ctx->TRUE()) return Object(true);
    else if (ctx->FALSE()) return Object(false);
    else if (ctx->NUMBER()) {
        Object cur;
        std::string tmp = ctx->NUMBER()->toString();
        (tmp.find('.') == string::npos) ? (cur = BigInt(tmp)) : (cur = Object( std::stod (tmp)));
        return cur;
    }
    else if (ctx->NAME()) {
        Object name(ctx->NAME()->toString());
        if (FindVar(name).toBOOL())
            return GetVar(name)-> second;
        return name;
    }
    if (ctx->STRING(0)) {
        string cur;
        auto strings = ctx->STRING();
        for (auto itm: strings) {
            string tmp = itm->toString();
            tmp.erase(tmp.begin());
            tmp.pop_back();
            cur += tmp;
        }
        return Object(cur);
    }
    if (ctx->test()) return visit(ctx->test());
}
antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
    auto tests = ctx->test();
    List cur;
    for (auto &test: tests)
            cur.push_back(visit(test).as<Object>());
    if (cur.size() == 1 && cur[0].type_py == LIST)
        return cur[0];
    return Object(cur);
}
antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
    auto arguments = ctx->argument();
    List cur;
    for (auto & argument : arguments)
    cur.push_back(visit(argument).as<Object>());
    if (cur.size()==1 && cur[0].type_py == LIST) return cur[0];
    return  Object(cur);
}
antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
    return visit(ctx->test());
}


//
EvalVisitor::Ob_Map &EvalVisitor::Glo_Vars() {
    return vars.front();
}
EvalVisitor::Ob_Map &EvalVisitor::Cur_Vars() {
    return vars.back();
}
Object EvalVisitor::FindVar (const Object &name) {
    bool flag = (Glo_Vars().find(name.toSTRING()) != Glo_Vars().end()) ||
            (Cur_Vars().find(name.toSTRING()) != Glo_Vars().end());
    return Object(flag);
}
std::unordered_map<std::string, Object>::iterator EvalVisitor::GetVar(const Object &name) {
    auto pos = Cur_Vars().find(name.toSTRING());
    if (pos != Cur_Vars().end()) return pos;
    return Glo_Vars().find(name.toSTRING());
}
Object & EvalVisitor::Insert_Var(const Object &name, const Object &value) {
    Cur_Vars().insert({name.toSTRING(),value});
    return Cur_Vars().find(name.toSTRING())->second;
}
Object & EvalVisitor::Assign_Var(const Object &name, const Object &value) {
    if (FindVar(name.toSTRING()).toBOOL())
        return GetVar(name)->second = value;
    return Insert_Var(name, value);
}

