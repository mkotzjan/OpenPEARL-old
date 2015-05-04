/*
 * [The "BSD license"]
 *  Copyright (c) 2012-2014 Marcel Schaible
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
import org.stringtemplate.v4.STGroup;
import org.stringtemplate.v4.STGroupFile;
import java.util.ArrayList;
import java.util.LinkedList;
import javax.xml.parsers.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;
import org.xml.sax.*;
import org.w3c.dom.*;

public class SystemPartExporter extends SmallPearlBaseVisitor<ST> implements SmallPearlVisitor<ST> {

    private static final String SYSTEMPART_EXPORT_STG = "SystemPartExport.stg";

    private STGroup group;
    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;


    public SystemPartExporter(String sourceFileName, int verbose, boolean debug) {

        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;

        if (m_verbose > 1) {
            System.out.println("Generating SystemPart definitions");
        }

        this.ReadTemplate(SYSTEMPART_EXPORT_STG);
    }

    private Void ReadTemplate(String filename) {
        if (m_verbose > 1) {
            System.out.println("Read StringTemplate Group File: " + filename);
        }

        this.group = new STGroupFile(filename);

        return null;
    }

    @Override
    public ST visitModule(SmallPearlParser.ModuleContext ctx) {
        ST module = group.getInstanceOf("Module");

        module.add( "sourcefile", m_sourceFileName);
        module.add( "name", ctx.ID().getText());
        System.out.println( "SystemPartExporter: visitModule");
        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.System_partContext) {
                    module.add("SystemPart", visitSystem_part((SmallPearlParser.System_partContext) c));
                }
            }
        }

        return module;
    }

    @Override
    public ST visitSystem_part(SmallPearlParser.System_partContext ctx) {
        ST st = group.getInstanceOf("SystemPart");

        System.out.println( "SystemPartExporter: visitSystem_part");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.Username_declarationContext) {
                    st.add("decls", visitUsername_declaration((SmallPearlParser.Username_declarationContext) c));
                }
            }
        }

        return st;
    }

    @Override
    public ST visitUsername_declaration(SmallPearlParser.Username_declarationContext ctx) {
        ST decl = group.getInstanceOf("Username_Declaration");

        System.out.println("SystemPartExporter: visitUsername_declaration");

        decl.add("var", ctx.ID().getText());

        if ( ctx.username_declaration_with_data_flow_direction() != null )
        {
           decl.add("type", visitUsername_declaration_with_data_flow_direction(ctx.username_declaration_with_data_flow_direction()));
        }
        else if ( ctx.username_declaration_without_data_flow_direction() != null )
        {
            decl.add("type", visitUsername_declaration_without_data_flow_direction(ctx.username_declaration_without_data_flow_direction()));
        }

        return decl;
    }

    @Override
    public ST visitUsername_declaration_without_data_flow_direction(SmallPearlParser.Username_declaration_without_data_flow_directionContext  ctx) {
        ST decl = group.getInstanceOf("Username_Declaration_Without_Dataflow_Direction");

        System.out.println("SystemPartExporter: visitUsername_declaration_without_data_flow_direction");

        decl.add("peripheral_name", ctx.peripheral_name().ID().getText());

        if (ctx.username_parameters() != null) {
            decl.add("username_parameters", visitUsername_parameters(ctx.username_parameters()));
        }
        return decl;
    }


    // username_declaration_with_data_flow_direction:
    // data_flow_direction ':' connection_name '---' peripheral_name username_parameters?


    @Override
    public ST visitUsername_declaration_with_data_flow_direction(SmallPearlParser.Username_declaration_with_data_flow_directionContext  ctx) {
        ST decl = group.getInstanceOf("Username_Declaration_With_Dataflow_Direction");

        System.out.println("SystemPartExporter: visitUsername_declaration_with_data_flow_direction");

        decl.add("data_flow_direction", ctx.data_flow_direction().getText());
        decl.add("connection", ctx.connection_name().getText());
        decl.add("peripheral_name", ctx.peripheral_name().ID().getText());

        if (ctx.username_parameters() != null) {
            decl.add("username_parameters", visitUsername_parameters(ctx.username_parameters()));
        }

        return decl;
    }


    @Override
    public ST visitUsername_parameters(SmallPearlParser.Username_parametersContext  ctx) {
        ST params = group.getInstanceOf("Username_Parameters");

        System.out.println("SystemPartExporter: visitUsername_parameters");

        for (int i = 0; i < ctx.literal().size(); i++) {
            String p = ctx.literal(i).getText();
            p = p.replaceAll("^'","");
            p = p.replaceAll("'$","");
            params.add("params",p);
        }

        return params;
    }


}

