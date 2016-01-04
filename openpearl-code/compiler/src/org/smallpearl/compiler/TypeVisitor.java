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

import org.antlr.v4.runtime.tree.ParseTree;
import org.stringtemplate.v4.ST;

import java.util.ArrayList;

public class TypeVisitor extends SmallPearlBaseVisitor<TypeDefinition> implements SmallPearlVisitor<TypeDefinition> {

    private int m_verbose;

    public TypeVisitor(int verbose) {
        m_verbose = verbose;
    }

    @Override
    public TypeDefinition visitFormalParameter(SmallPearlParser.FormalParameterContext ctx) {
        TypeDefinition typeDef = null;
        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                typeDef = visitParameterType(ctx.parameterType());
            }
        }

        return typeDef;
    }

    @Override
    public TypeDefinition visitParameterType(SmallPearlParser.ParameterTypeContext ctx) {
        TypeDefinition typeDef = null;
        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.SimpleTypeContext) {
                typeDef = visitSimpleType(ctx.simpleType());
            }
        }

        return typeDef;
    }

    @Override
    public TypeDefinition visitSimpleType(SmallPearlParser.SimpleTypeContext ctx) {
        TypeDefinition typeDef = null;
        if (ctx != null) {
            if (ctx.typeInteger() != null) {
                typeDef = visitTypeInteger(ctx.typeInteger());
            }
        }

        return typeDef;
    }

    @Override
    public TypeDefinition visitTypeInteger(SmallPearlParser.TypeIntegerContext ctx) {
        Integer size = 31;

        if ( ctx.mprecision() != null ) {
            size = Integer.parseInt(ctx.mprecision().integerWithoutPrecision().IntegerConstant().getText());
        }

        return new TypeFixed(size);
    }

}