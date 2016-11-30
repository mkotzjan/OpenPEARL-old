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

    private static final String IMC_EXPORT_STG = "IMC.stg";

    private STGroup group;
    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;

    public SystemPartExporter(String sourceFileName, int verbose, boolean debug) {

        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;

        if (m_verbose > 1) {
            System.out.println("Generating InterModuleChecker definitions");
        }

        this.ReadTemplate(IMC_EXPORT_STG);
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

        module.add("sourcefile", m_sourceFileName);
        module.add("name", ctx.ID().getText());

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.System_partContext) {
                    module.add("SystemPart", visitSystem_part((SmallPearlParser.System_partContext) c));
                } else if (c instanceof SmallPearlParser.Problem_partContext) {
                    module.add("ProblemPart", visitProblem_part((SmallPearlParser.Problem_partContext) c));
                }
            }
        }
        
        return module;
    }

    @Override
    public ST visitSystem_part(SmallPearlParser.System_partContext ctx) {
        ST st = group.getInstanceOf("SystemPart");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.Username_declarationContext) {
                    st.add("decls", visitUsername_declaration((SmallPearlParser.Username_declarationContext) c));
                } else if (c instanceof SmallPearlParser.UserConfigurationWithoutAssociationContext) {
                    st.add("decls", visitUserConfigurationWithoutAssociation((SmallPearlParser.UserConfigurationWithoutAssociationContext) c));
                } else if (c instanceof SmallPearlParser.UserConfigurationWithAssociationContext) {
                    st.add("decls", visitUserConfigurationWithAssociation((SmallPearlParser.UserConfigurationWithAssociationContext) c));
                }
            }
        }

        return st;
    }

    @Override
    public ST visitUsername_declaration(SmallPearlParser.Username_declarationContext ctx) {
        ST decl = group.getInstanceOf("Username_Declaration");

        decl.add("username", ctx.ID().getText());
        decl.add("lineno", ctx.start.getLine());

        if (ctx.username_declaration_with_data_flow_direction() != null) {
            decl.add("decl", visitUsername_declaration_with_data_flow_direction(ctx.username_declaration_with_data_flow_direction()));
        } else if (ctx.username_declaration_without_data_flow_direction() != null) {
            decl.add("decl", visitUsername_declaration_without_data_flow_direction(ctx.username_declaration_without_data_flow_direction()));
        }

        return decl;
    }

    @Override
    public ST visitUsername_declaration_without_data_flow_direction(SmallPearlParser.Username_declaration_without_data_flow_directionContext ctx) {
        ST decl = group.getInstanceOf("Username_Declaration_Without_Dataflow_Direction");

        decl.add("lineno", ctx.start.getLine());
        decl.add("sysname", ctx.ID().toString());

        if (ctx.username_parameters() != null) {
            decl.add("parameters", visitUsername_parameters(ctx.username_parameters()));
        }

        return decl;
    }

    @Override
    public ST visitUsername_declaration_with_data_flow_direction(SmallPearlParser.Username_declaration_with_data_flow_directionContext ctx) {
        ST decl = group.getInstanceOf("Username_Declaration_With_Dataflow_Direction");

        decl.add("sysname", ctx.ID(0).toString());

        if (ctx.username_parameters() != null) {
            decl.add("parameters", visitUsername_parameters(ctx.username_parameters(0)));

            ST association = group.getInstanceOf("Association");
            association.add("name", ctx.ID(1).toString());
            if (ctx.username_parameters().size() > 1) {
                association.add("parameters", visitUsername_parameters(ctx.username_parameters(1)));
            }

            decl.add("association", association);
        }

        return decl;
    }


    @Override
    public ST visitUsername_parameters(SmallPearlParser.Username_parametersContext ctx) {
        ST parameters = group.getInstanceOf("Username_Parameters");

        for (int i = 0; i < ctx.literal().size(); i++) {
            ST parameter = group.getInstanceOf("Parameter");
            String param = ctx.literal(i).getText();
//            param = param.replaceAll("^'","");
//            param = param.replaceAll("'$","");

            if (ctx.literal(i).StringLiteral() != null) {
                ST type = group.getInstanceOf("Type_Char");
                type.add("name", param);
                parameter.add("type", type);
            } else if (ctx.literal(i).BitStringLiteral() != null) {
                ST type = group.getInstanceOf("Type_Bit");
                type.add("name", param);
                parameter.add("type", type);
            } else if (ctx.literal(i).IntegerConstant() != null) {
                ST type = group.getInstanceOf("Type_Fixed");
                type.add("name", param);
                parameter.add("type", type);
            }

            parameters.add("params", parameter);
        }

        return parameters;
    }


    @Override
    public ST visitUserConfigurationWithoutAssociation(SmallPearlParser.UserConfigurationWithoutAssociationContext ctx) {
        ST user_configuration = group.getInstanceOf("User_Configuration");

        user_configuration.add("lineno", ctx.start.getLine());
        user_configuration.add("sysname", ctx.user_configuration_without_association().ID().toString());

        if (ctx.user_configuration_without_association().username_parameters() != null) {
            user_configuration.add("parameters", visitUsername_parameters(ctx.user_configuration_without_association().username_parameters()));
        }


        return user_configuration;
    }

    @Override
    public ST visitUserConfigurationWithAssociation(SmallPearlParser.UserConfigurationWithAssociationContext ctx) {
        ST user_configuration = group.getInstanceOf("User_Configuration");

        user_configuration.add("lineno", ctx.start.getLine());
        user_configuration.add("sysname", ctx.user_configuration_with_association().ID(0).toString());

        if (ctx.user_configuration_with_association().username_parameters() != null) {
            user_configuration.add("parameters", visitUsername_parameters(ctx.user_configuration_with_association().username_parameters()));
        }

        ST association = group.getInstanceOf("Association");
        association.add("name", ctx.user_configuration_with_association().ID(1).toString());
        user_configuration.add("association", association);

        return user_configuration;
    }

    @Override
    public ST visitProblem_part(SmallPearlParser.Problem_partContext ctx) {
        ST st = group.getInstanceOf("ProblemPart");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.IdentificationContext) {
                    st.add("decls", visitIdentification((SmallPearlParser.IdentificationContext) c));
                } else if (c instanceof SmallPearlParser.DationSpecificationContext) {
                    st.add("decls", visitDationSpecification((SmallPearlParser.DationSpecificationContext) c));
                }
            }
        }

        return st;
    }

    @Override
    public ST visitIdentification(SmallPearlParser.IdentificationContext ctx) {
        ST st = group.getInstanceOf("Specification");

        st.add("lineno", ctx.start.getLine());
        st.add("name", ctx.ID().toString());
        if (ctx.type() != null) {
            st.add("type", visitType(ctx.type()));
        }
        return st;
    }

    @Override
    public ST visitDationSpecification(SmallPearlParser.DationSpecificationContext ctx) {
        ST dationSpecification = group.getInstanceOf("DationSpecification");

        dationSpecification.add("lineno", ctx.start.getLine());

        boolean hasGlobalAttribute = false;

        ArrayList<String> identifierDenotationList = null;
        if (ctx != null) {
            if (ctx.identifierDenotation() != null ){
                identifierDenotationList = getIdentifierDenotation(ctx.identifierDenotation());
            }

            if ( ctx.globalAttribute() != null ) {
                hasGlobalAttribute = true;
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                dationSpecification.add( "name", identifierDenotationList.get(i));
            }

            if ( ctx.specifyTypeDation() != null ) {
                ST datalist = group.getInstanceOf("DataList");
                ST attributes = group.getInstanceOf("Attributes");

                if (ctx.specifyTypeDation().classAttribute() != null) {
                    if (ctx.specifyTypeDation().classAttribute().alphicDation() != null) {
                        ST data = group.getInstanceOf("Data");
                        data.add("name", "ALPHIC");
                        datalist.add("data",data);
                    }

                    if (ctx.specifyTypeDation().classAttribute().basicDation() != null) {
                        ST attribute = group.getInstanceOf("Attribute");
                        attribute.add("name", "BASIC");
                        attributes.add("attributes", attribute);
                    }

                    if (ctx.specifyTypeDation().classAttribute().systemDation() != null) {
                        ST attribute = group.getInstanceOf("Attribute");
                        attribute.add("name", "SYSTEM");
                        attributes.add("attributes", attribute);
                    }

                    if ( ctx.specifyTypeDation().classAttribute() != null ) {
                        if ( ctx.specifyTypeDation().classAttribute().typeOfTransmissionData() != null) {
                            ST data = group.getInstanceOf("Data");
                            data.add("name", ctx.specifyTypeDation().classAttribute().typeOfTransmissionData().getText());
                            datalist.add("data",data);
                        }
                    }
                }

                if ( ctx.specifyTypeDation().sourceSinkAttribute() != null) {
                    ST attribute = group.getInstanceOf("Attribute");
                    attribute.add("name", ctx.specifyTypeDation().sourceSinkAttribute().getText());
                    attributes.add("attributes",attribute);
                }

                dationSpecification.add("datalist", datalist);
                dationSpecification.add("attributes", attributes);
            }
        }

        return dationSpecification;
    }

    @Override
    public ST visitType(SmallPearlParser.TypeContext ctx) {
        ST type = group.getInstanceOf("Type");

        if ( ctx.simple_type() != null) {
            if ( ctx.simple_type().type_bit() != null ) {
                type.add("name", "BIT");
            }
            else if ( ctx.simple_type().type_char() != null ) {
                type.add("name", "CHAR");
            }
            else if ( ctx.simple_type().type_fixed() != null ) {
                type.add("name", "FIXED");
            }
            else if ( ctx.simple_type().type_float() != null ) {
                type.add("name", "FLOAT");
            }
            else {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }
        else if ( ctx.type_realtime_object() != null) {
                type.add("name", ctx.type_realtime_object().getText());
        }
        else if ( ctx.typeTime() != null) {
            if ( ctx.typeTime().type_clock() != null ) {
                type.add("name", "CLOCK");
            }
            else if ( ctx.typeTime().type_duration() != null ) {
                type.add("name", "DURATION");
            }
            else {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return type;
    }


    private ArrayList<String> getIdentifierDenotation(SmallPearlParser.IdentifierDenotationContext ctx) {
        ArrayList<String> identifierDenotationList = new ArrayList<String>();

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                identifierDenotationList.add(ctx.ID().get(i).toString());
            }
        }

        return identifierDenotationList;
    }
}