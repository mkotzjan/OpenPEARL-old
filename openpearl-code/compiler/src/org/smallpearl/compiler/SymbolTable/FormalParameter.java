package org.smallpearl.compiler.SymbolTable;

import org.smallpearl.compiler.TypeDefinition;

public class FormalParameter {

    public  String          name;
    public  TypeDefinition  type;
    public  Boolean         assignmentProtection;

    public FormalParameter() {
    }

    public FormalParameter(String name, TypeDefinition type, Boolean assignmentProtection) {
        this.name = name;
        this.type = type;
        this.assignmentProtection = assignmentProtection;
    }

    public String toString() {
        return (this.assignmentProtection ? " INV " : " " ) + this.type;
    }


}