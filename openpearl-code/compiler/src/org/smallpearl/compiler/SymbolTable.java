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

import java.util.Iterator;
import java.util.LinkedList;

/**
 * Symboltable is ...
 * This information includes:
 * <ul>
 * <li>The Component to draw on
 * <li>A translation origin for rendering and clipping coordinates
 * (see <a href="#setXORMode">setXORMode</a>)
 * </ul>
 * <p/>
 * Coordinates are infinitely thin and lie between the pixels of the
 * output device.
 * <p/>
 *
 * @author Marcel Schaible
 */

public class SymbolTable {
    private ScopeStack scopeStack = new ScopeStack();
    private LinkedList unknowns = new LinkedList();
    private LinkedList<TaskDef> tasks = new LinkedList();
    private LinkedList<TemporarySemaphoreArray>  temporarySemaphoreArrays = new LinkedList();

    private ScopeDef baseScope;
    private static int scopeCounter = 0;

    private static SymbolTable symbolTable = null;

    private SymbolTable() {
        scopeStack.push(baseScope);
    }

    public static SymbolTable getSymbolTable() {
        if (symbolTable == null) {
            symbolTable = new SymbolTable();
        }
        return symbolTable;
    }

    public void reset() {
        scopeCounter = 0;
        symbolTable = new SymbolTable();
    }

    public static int getScopeCounter() {
        return scopeCounter++;
    }

    public ScopeDef getBaseScope() {
        return baseScope;
    }

    public Definition lookup(String s) {
        Definition def = scopeStack.lookup(s);
        return def;
    }

    public int[] lookupPos(String s) {
        return scopeStack.lookupPos(s);
    }

    public void identifierUsage(Symbol s) {
        if (lookup(s.getName()) == null) {
            addUnknownDef(s);
        }
    }

    private void addUnknownDef(Symbol s) {
        Position pos = new Position(s.getLine(), s.getColumn());
        UnknownDef unknown = new UnknownDef(s.getName(), pos, getCurrentScope());
        unknowns.add(unknown);
    }

    public void defineModule(Symbol s) {
        Position occ = new Position(s.getLine(), s.getColumn());
        baseScope = new ModuleDef(s.getName(), occ, getCurrentScope());
        pushScope(baseScope);
    }

    public void defineProcedure(Symbol s) {
        Position occ = new Position(s.getLine(), s.getColumn());
        ProcedureDef procedure = new ProcedureDef(s.getName(), occ, getCurrentScope());
        addToCurrentScope(procedure);
        pushScope(procedure);
    }

    public void defineTask(Symbol s, int priority) {
        Position occ = new Position(s.getLine(), s.getColumn());
        TaskDef task = new TaskDef(s.getName(), occ, getCurrentScope(), priority);
        tasks.add(task);
        addToCurrentScope(task);
        pushScope(task);
    }

    public void defineVar(Symbol s) {
        Position occ = new Position(s.getLine(), s.getColumn());
        VariableDef variable = new VariableDef(s.getName(), occ, getCurrentScope());
        addToCurrentScope(variable);
    }

    public void defineFixedVar(Symbol s, int prec) {
        Position occ = new Position(s.getLine(), s.getColumn());
        FixedVariableDef variable = new FixedVariableDef(s.getName(), prec, occ, getCurrentScope());
        addToCurrentScope(variable);
    }

    public void defineCharVar(Symbol s, int length) {
        Position occ = new Position(s.getLine(), s.getColumn());
        CharVariableDef variable = new CharVariableDef(s.getName(), length, occ, getCurrentScope());
        addToCurrentScope(variable);
    }

    public void defineFloatVar(Symbol s, int prec) {
        Position occ = new Position(s.getLine(), s.getColumn());
        FloatVariableDef variable = new FloatVariableDef(s.getName(), prec, occ, getCurrentScope());
        addToCurrentScope(variable);
    }

    public void defineConst(Symbol s, int value) {
        Position occ = new Position(s.getLine(), s.getColumn());
        ConstantDef constant = new ConstantDef(s.getName(), value, occ, getCurrentScope());
        addToCurrentScope(constant);
    }

    private void addToCurrentScope(Definition def) {
        getCurrentScope().add(def);
        def.setParentScope(getCurrentScope());
    }

    public void resolveUnknown() {
        for (Iterator it = unknowns.iterator(); it.hasNext(); ) {
            UnknownDef ud = (UnknownDef) it.next();
            Definition looked = ud.getParentScope().lookup(ud.getName());
            Symbol s = new Symbol(ud.getName(), ud.getPosition().getLine(),
                    ud.getPosition().getColumn(), -1);
            if (looked == null) {
                throw new NoDeclarationException(s);
            }
        }
    }

    private ScopeDef getCurrentScope() {
        if (scopeStack.empty())
            return null;
        return (ScopeDef) scopeStack.peek();
    }

    public void popScope() {
        scopeStack.pop();
    }

    public void pushScope(ScopeDef scope) {
        scopeStack.push(scope);
    }

    public void checkForVariable(Symbol s) {
        Definition d = lookup(s.getName());
        if (d == null) {
            throw new NoDeclarationException(s);
        }
        if (!(d instanceof VariableDef)) {
            throw new NoVariableException(s.getName(), s.getLine(), s.getColumn(), d.getType());
        }
    }

    public void checkForProcedure(Symbol s) {
        Definition d = lookup(s.getName());
        if (d == null) {
            addUnknownDef(s);
        } else {
            if (!(d instanceof ProcedureDef)) {
                throw new NoProcedureException(s.getName(), s.getLine(), s.getColumn(), d.getType());
            }
        }
    }

    public void checkForTask(Symbol s) {
        Definition d = lookup(s.getName());
        if (d == null || !(d instanceof TaskDef)) {
            throw new NoTaskException(s.getName(), s.getLine(), s.getColumn());
        }
    }

    public void checkForValue(Symbol s) {
        Definition d = lookup(s.getName());
        if (d == null) {
            throw new NoDeclarationException(s);
        }
        if (!(d instanceof VariableDef) && !(d instanceof ConstantDef)) {
            throw new NoValueException(s.getName(), s.getLine(), s.getColumn(), d.getType());
        }
    }

    public String toString() {
        return "Symboltable:" +
                "\n  ScopeStack:" + scopeStack +
                "\n  unknowns:" + unknowns +
                "\n  baseScope:" + baseScope +
                "\n  scopeCounter:" + scopeCounter +
                "\n  tasks:" + tasks;

    }

    public LinkedList<TaskDef> getTasks() {
        return tasks;
    }

    public void getGlobalsDeclarations() {
        for (Iterator it = unknowns.iterator(); it.hasNext(); ) {
            System.out.println(it);

            VariableDef vd = (VariableDef) it.next();
            System.out.println("****** " + vd);

/*
            Definition looked = ud.getParentScope().lookup(ud.getName());
            Symbol s = new Symbol(ud.getName(), ud.getPosition().getLine(),
                    ud.getPosition().getColumn(), -1);
            if (looked == null) {
                throw new NoDeclarationException(s);
            }
*/
        }
    }

    public LinkedList<TemporarySemaphoreArray>  getTemporarySemaphoreArrays() { return temporarySemaphoreArrays; }


    public Void defineTemporarySemaphoreArray( TemporarySemaphoreArray arr) {
        boolean found = false;
        int i = 0;

        while ( !found && i <temporarySemaphoreArrays.size()) {
            found = temporarySemaphoreArrays.get(i).getArrayName().equals(arr.getArrayName());
            i++;
        }

        if ( !found ) {
            temporarySemaphoreArrays.add(arr);
        }
        return null;
    }
    public ScopeDef getGlobalScope() {
        if (scopeStack.empty())
            return null;

        return (ScopeDef) scopeStack.elementAt(1);
    }

    public Definition xxlookup(String s) {
        Definition def = scopeStack.lookup(s);
        return def;
    }

    public void defineBlock(Symbol s) {
        Position occ = new Position(s.getLine(), s.getColumn());
        BlockDef block = new BlockDef(occ, getCurrentScope());
        addToCurrentScope(block);
        pushScope(block);
    }

}
