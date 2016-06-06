#include <iostream>
#include <string>

#include "Node.h"
#include "Operation.h"

Node::Node(NodeType t) {
    type = t;
}

string Node::toString() {
    if (type == nodeTypeOption) {
        return "option";
    } else {
        return "operation";
    }
}
