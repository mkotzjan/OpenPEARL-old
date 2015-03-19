/*
 * [The "BSD license"]
 *  Copyright (c) 2012-2013 Marcel Schaible
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

import org.antlr.v4.runtime.Token;

public class Symbol {

    public static final int SYMBOL_CONST = 0;
    public static final int SYMBOL_VAR = 1;
    public static final int SYMBOL_PROC = 2;
    public static final int SYMBOL_TASK = 3;
    public static final int SYMBOL_MODULE = 4;
    public static final int SYMBOL_SEMAPHORE = 5;

    public static final String[] SYMBOLNAME_LC = new String[]{"constant", "variable", "procedure", "task", "module", "semaphore"};
    public static final String[] SYMBOLNAME_UC = new String[]{"Constant", "Variable", "Procedure", "Task", "Module", "Semaphore"};

    private String name;
    private int type;
    private int line;
    private int column;

    public Symbol(Token t, int type) {
        this(t.getText(), t.getLine(), t.getCharPositionInLine(), type);
    }

    public Symbol(Token t) {
        this(t, -1);
    }

    public Symbol(String name, int line, int column, int type) {
        this.name = name;
        this.type = type;
        this.line = line;
        this.column = column;
    }

    public String getName() {
        return name;
    }

    public int getType() {
        return type;
    }

    public int getLine() {
        return line;
    }

    public int getColumn() {
        return column;
    }

    public String toString() {
        return "(" + name + ", " + SYMBOLNAME_LC[type] + " at " + line + ":" + column + ")";
    }
}
