#include "AptaBasedAlgorithm.h"

AptaBasedAlgorithm::AptaBasedAlgorithm(Apta apta, bool buildVisualizations, string visualizationPrefix)
{
    this->_apta = apta;
    this->_buildVisualizations = buildVisualizations;
    this->_visualizationPrefix = visualizationPrefix;
}

void AptaBasedAlgorithm::_buildVisualization(string outputFileName)
{
    string outputName = this->_visualizationPrefix +
        to_string(this->_visualizationId) + "_" + outputFileName;
    AptaVisualization aptaVisualization(outputName.c_str());
    aptaVisualization.build(this->_apta);
    this->_visualizationId++;
}

void AptaBasedAlgorithm::_switchParents(string blueNodeId, string redNodeId)
{
    Apta::NodeEdges & nodeEdges = this->_apta.getNodeEdges();
    Apta::NodeEdges & nodeEdges2 = this->_apta.getNodeEdges2();

    Apta::NodeEdges::iterator & blueNodesParentsIterator = nodeEdges2.find(blueNodeId);
    if (blueNodesParentsIterator == nodeEdges2.end()) { // Not found
        assert(false && "This should never happen! Blue node has no parents!");
    }

    Apta::NodeChildren & blueNodesParents = blueNodesParentsIterator->second;
    Apta::NodeChildren::iterator blueNodeParent;
    for (Apta::NodeChildren::iterator & blueNodeParent = blueNodesParents.begin();
        blueNodeParent != blueNodesParents.end();
    ) {
        Apta::NodeChildren & localNodeChildren = nodeEdges.find(blueNodeParent->second)->second;
        Apta::NodeChildren::iterator & localNodeChild =
            localNodeChildren.find({ blueNodeParent->first, blueNodeId });
        localNodeChildren.erase(localNodeChild);
        localNodeChildren.insert({ blueNodeParent->first, redNodeId });

        nodeEdges2.find(redNodeId)->second.insert({ blueNodeParent->first, blueNodeParent->second });
        blueNodesParents.erase(blueNodeParent++);
    }
}

void AptaBasedAlgorithm::_switchChildren(string blueNodeId, string redNodeId, bool colorBlue)
{
    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes & whiteNodes = this->_apta.getWhiteNodes();

    Apta::NodeEdges & nodeEdges = this->_apta.getNodeEdges();
    Apta::NodeEdges & nodeEdges2 = this->_apta.getNodeEdges2();

    Apta::NodeEdges::iterator & blueNodesChildrenIterator = nodeEdges.find(blueNodeId);
    if (blueNodesChildrenIterator == nodeEdges.end()) { // Not found
        return;
    }

    Apta::NodeChildren localRedNodesChildren;

    Apta::NodeChildren & blueNodesChildren = blueNodesChildrenIterator->second;
    for (Apta::NodeChildren::iterator & blueNodesChild = blueNodesChildren.begin();
        blueNodesChild != blueNodesChildren.end();
    ) {

        // Color them to blue
        if (colorBlue == true) {
            string blueNodeLabel = this->_apta.getLabelByNodeId(blueNodesChild->second);
            blueNodes.insert({ blueNodesChild->second, blueNodeLabel });
            whiteNodes.erase(blueNodesChild->second);
        }

        Apta::NodeEdges::iterator & redNodesChildren = nodeEdges.find(redNodeId);

        if (redNodesChildren != nodeEdges.end()) { // Found
            pair<char, string> newChild = make_pair(blueNodesChild->first, blueNodesChild->second);
            redNodesChildren->second.insert(newChild);
        } else {
            localRedNodesChildren.clear();
            localRedNodesChildren.insert({ blueNodesChild->first, blueNodesChild->second });
            nodeEdges.insert({ redNodeId, localRedNodesChildren });
        }

        Apta::NodeChildren & localBlueNodesParent = nodeEdges2.find(blueNodesChild->second)->second;
        localBlueNodesParent.insert({ blueNodesChild->first, redNodeId });
        localBlueNodesParent.erase({ blueNodesChild->first, blueNodeId });

        blueNodesChildren.erase(blueNodesChild++);
    }
}

AptaBasedAlgorithm::~AptaBasedAlgorithm()
{
}
