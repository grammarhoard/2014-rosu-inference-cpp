#include "MinimallyAdequateTeacher.h"

MinimallyAdequateTeacher::MinimallyAdequateTeacher(Language& L): _L(L)
{
}

MinimallyAdequateTeacher::~MinimallyAdequateTeacher()
{
}

Language& MinimallyAdequateTeacher::getLanguage()
{
    return this->_L;
}

bool MinimallyAdequateTeacher::Mem(const string l, const string u, const string r)
{
    return this->_L.in(l + u + r);
}

bool MinimallyAdequateTeacher::Equiv(const ContextFreeGrammar& G)
{
    // The learner provides the teacher with a hypothesis, and
    //     the teacher will either confirm that it is correct, or
    //     it will provide the learner with a counter - example

    /* Info:
    We will make equivalence queries where the hypothesis may not be in the learnable class
        - this is sometimes called an Extended Minimally Adequate Teacher [5].
    We can consider the equivalent construction for CFGs to be
        where we have a correspondence between the congruence classes of the language
        and the non-terminals of the grammar (Section 2)
    Equivalence relation - have the same distribution -> equivalence classes
    Categories - ...

    The equivalence of NTS grammars is decidable [13], and
        it is polynomially decidable whether a given CFG is NTS [14]

    We have implemented the algorithm using a sampling approximation to the equivalence oracle and,
        though we do not present any experimental results here, it is simple and efficient. More generally,
        one can approximate the equivalence oracle by generating examples from a
        fixed distribution or from both target and hypothesis grammars, either using a
        probabilistic CFG or otherwise[15].

    [2] Dana Angluin, Learning regular sets from queries and counterexamples
    */

    //TODO implement MinimallyAdequateTeacher::Equiv()
    if (this->_counterExample == "") {
        this->_counterExample = "ab";
    } else if (this->_counterExample == "ab") {
        this->_counterExample = "aab";
    } else {

        this->_counterExample = "";
        return true;
    }
    return false;
}

string MinimallyAdequateTeacher::getCounterExample()
{
    return this->_counterExample;
}
