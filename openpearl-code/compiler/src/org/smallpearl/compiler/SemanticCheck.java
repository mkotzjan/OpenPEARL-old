/*
 * [The "BSD license"]
 *  Copyright (c) 2012-2016 Marcel Schaible
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.smallpearl.compiler;

import org.antlr.v4.runtime.ParserRuleContext;
import org.smallpearl.compiler.SymbolTable.ModuleEntry;
import org.smallpearl.compiler.SymbolTable.SymbolTable;
import org.smallpearl.compiler.SemanticAnalysis.*;

public class SemanticCheck {

    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;
    private ExpressionTypeVisitor m_expressionTypeVisitor;
    private SymbolTableVisitor m_symbolTableVisitor;
    private SymbolTable m_symboltable;
    private SymbolTable m_currentSymbolTable;
    private ModuleEntry m_module;
    private ParserRuleContext m_parseTree;

    public SemanticCheck(String sourceFileName,
                         int verbose,
                         boolean debug,
                         ParserRuleContext tree,
                         SymbolTableVisitor symbolTableVisitor,
                         ExpressionTypeVisitor expressionTypeVisitor) {
        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;
        m_symbolTableVisitor = symbolTableVisitor;
        m_expressionTypeVisitor = expressionTypeVisitor;
        m_symboltable = symbolTableVisitor.symbolTable;
        m_parseTree = tree;

        if (m_verbose > 0) {
            System.out.println( "Performing semantic check");
        }

        new CheckVariableDeclaration(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);
        new CheckAssignment(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);
        new CheckProcedureCall(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);
        new CheckCondition(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);
        new CheckSwitchCase(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);
        new CheckInterrupt(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);
        new CheckDeclarationScope(m_sourceFileName, m_verbose, m_debug, m_symbolTableVisitor, m_expressionTypeVisitor).visit(m_parseTree);

    }

}