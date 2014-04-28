/*
 * The right side of a production rule
 * Because the Grammar is in Chomsky Normal Form (CNF),
 *     the right side can either by non-terminal non-terminal or terminal or lambda
 */
#pragma once
#include <string>

using namespace std;

class Terminal;
class NonTerminalNonTerminal;
class ProductionRight
{
public:

    /*
     * A name in needed in order to implement operator < (used by the set constructor)
     */
    ProductionRight(const string name);
    virtual ~ProductionRight();

    string getName() const;

    /*
     * We need this operator to add a ProductionRight to a set
     */
    bool operator < (const ProductionRight& productionRight) const;

    /*
     * Returns true only if the ProductionRight is a terminal and has the same name
     */
    virtual bool equals(const Terminal& terminal) const = 0;

    /*
     * Returns true only if the ProductionRight is a non-terminal non-terminal and has the same names
     */
    virtual bool equals(const NonTerminalNonTerminal& nonTerminalNonTerminal) const = 0;

private:
    string _name;
};
