/*
 * The right side of a production rule
 * Because the Grammar is in Chomsky Normal Form (CNF),
 *     the right side can either by non-terminal non-terminal or terminal or lambda
 */

#include <string>

using namespace std;

#pragma once
class ProductionRight
{
public:
    ProductionRight(const string name);
    virtual ~ProductionRight();

    string getName() const;

    /*
     * We need this operator to add a ProductionRight to a set
     */
    bool operator < (const ProductionRight& productionRight) const;

    /*
     * Compare the names of two Production Right elements
     */
    bool operator == (const ProductionRight& productionRight) const;

private:
    const string _name;
};
