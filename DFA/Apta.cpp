#include "Apta.h"

Apta::Apta()
{
}

void Apta::build(TrainingSet trainingSet, bool useWhiteNodes)
{
    for (pair<string, bool> sample : trainingSet.get()) {
        sample.first;
        sample.second;
    }

    /*
    // APTA init
    APTA = apta();
    APTA@redNodes = ();
    APTA@redNodesLabelList = [];
    APTA@blueNodes = ();
    APTA@whiteNodes = ();
    APTA@nodeEdges = ();
    APTA@nodeEdges2 = ();

    if (useWhite == true) {
        useWhiteNodes = true;
    }

    // Start with the root of APTA colored red
    addNewNode(true, rootId, "", "", "");

    for (tuple[str valueL, bool typeL] sample <- trainingSet@T) {
    addPath(rootId, sample.valueL, sample.typeL ? "Accepted" : "Rejected");
    }

    // Build APTA A
    /*
    set[str] Q, // set of node ids
    set[str] Z, // set of input symbols
    str(str nodeId, str edgeLabel) d, // transition function
    str s, // root node's id
    set[str] Fp, // final nodes of strings in S+
    set[str] Fm // final nodes of strings in S-

    APTA@A = <
    {id | id <-APTA@redNodes + APTA@blueNodes},
    alphabet,
    transitionFunction,
    rootId,
    { id | id <-APTA@redNodes, APTA@redNodes[id] == "Accepted" } +
    {id | id <-APTA@blueNodes, APTA@blueNodes[id] == "Accepted"},
    { id | id <-APTA@redNodes, APTA@redNodes[id] == "Rejected" } +
    {id | id <-APTA@blueNodes, APTA@blueNodes[id] == "Rejected"}
    >;
    return APTA;
    */
}

Apta::~Apta()
{
}
