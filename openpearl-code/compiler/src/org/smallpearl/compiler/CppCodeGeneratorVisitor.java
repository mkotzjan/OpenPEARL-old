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
import org.antlr.v4.runtime.tree.TerminalNodeImpl;
import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;
import org.stringtemplate.v4.STGroupFile;

import java.util.List;
import java.util.ArrayList;
import java.util.LinkedList;


public class CppCodeGeneratorVisitor extends SmallPearlBaseVisitor<ST> implements SmallPearlVisitor<ST> {

    private STGroup group;
    private int m_verbose;
    private boolean m_debug;
    private String m_sourceFileName;

    public enum Type {BIT, CHAR, FIXED}

    public static final double pi = java.lang.Math.PI;

    public CppCodeGeneratorVisitor(String sourceFileName, String filename, int verbose, boolean debug) {

        m_debug = debug;
        m_verbose = verbose;
        m_sourceFileName = sourceFileName;

        if (m_verbose > 1) {
            System.out.println("Generating Cpp code");
        }

        this.ReadTemplate(filename);
        generatePrologue();
    }

    private Void ReadTemplate(String filename) {
        if (m_verbose > 1) {
            System.out.println("Read StringTemplate Group File: " + filename);
        }

        this.group = new STGroupFile(filename);

        return null;
    }

    private ST generatePrologue() {
        ST prologue = group.getInstanceOf("Prologue");

        prologue.add("src", this.m_sourceFileName);

        ST taskspec = group.getInstanceOf("TaskSpecifier");

        SymbolTable symtab = SymbolTable.getSymbolTable();

        LinkedList<TaskDef> tasks = symtab.getTasks();

        ArrayList<String> t = new ArrayList();

        for (int i = 0; i < tasks.size(); i++) {
            TaskDef task = tasks.get(i);
            t.add(task.getName());
        }

        taskspec.add("taskname", t);
        prologue.add("taskSpecifierList", taskspec);

        return prologue;
    }

    private Double getDuration(SmallPearlParser.DurationConstantContext ctx) {
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;

        if (ctx.hours() != null) {
            hours = Integer.valueOf(ctx.hours().IntegerConstant().toString()) * 3600;
        }
        if (ctx.minutes() != null) {
            minutes = Integer.valueOf(ctx.minutes().IntegerConstant().toString()) * 60;
        }
        if (ctx.seconds() != null) {
            if (ctx.seconds().IntegerConstant() != null) {
                seconds = Double.valueOf(ctx.seconds().IntegerConstant().toString());
            } else if (ctx.seconds().FloatingPointConstant() != null) {
                seconds = Double.valueOf(ctx.seconds().FloatingPointConstant().toString());
            }
        }

        return (hours + minutes + seconds);
    }

    /*
    Time/Clock example:
           11:30:00    means 11.30
           15:45:3.5   means 15.45 and 3.5 seconds
           25:00:00    means 1.00
*/
    private Double getTime(SmallPearlParser.TimeConstantContext ctx) {
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;

        hours = (Integer.valueOf(ctx.getChild(0).toString()) % 24);
        minutes = Integer.valueOf(ctx.getChild(2).toString());
        seconds = Double.valueOf(ctx.getChild(4).toString());

        if (hours < 0 || minutes < 0 || minutes > 59) {
            throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return hours * 3600 + minutes * 60 + seconds;
    }

    @Override
    public ST visitModule(SmallPearlParser.ModuleContext ctx) {
        ST module = group.getInstanceOf("module");

        module.add("src", this.m_sourceFileName);
        module.add("name", ctx.ID().getText());
        module.add("prologue", generatePrologue());

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.System_partContext) {
                    module.add("SystemPart", visitSystem_part((SmallPearlParser.System_partContext) c));
                } else if (c instanceof SmallPearlParser.Problem_partContext) {
                    module.add("ProblemPart", visitProblem_part((SmallPearlParser.Problem_partContext) c));
                } else if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    ST decl = group.getInstanceOf("cpp_inline");
                    module.add("cpp_inlines", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                }
            }
        }

        return module;
    }

    // TODO: SYSTEM Part
    @Override
    public ST visitSystem_part(SmallPearlParser.System_partContext ctx) {
        ST st = group.getInstanceOf("SystemPart");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    ST decl = group.getInstanceOf("cpp_inline");
                    st.add("cpp_inlines", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                }
            }
        }

        return st;
    }

    @Override
    public ST visitType(SmallPearlParser.TypeContext ctx) {
        ST type = group.getInstanceOf("type");

        if (ctx.simple_type() != null) {
            type.add("simple_type", visitSimple_type(ctx.simple_type()));
        } else if (ctx.typeTime() != null) {
            type.add("TypeTime", visitTypeTime(ctx.typeTime()));
        }

        return type;
    }

    @Override
    public ST visitSimple_type(SmallPearlParser.Simple_typeContext ctx) {
        ST simple_type = group.getInstanceOf("simple_type");

        if (ctx.type_fixed() != null) {
            simple_type.add("type_fixed", visitType_fixed(ctx.type_fixed()));
        } else if (ctx.type_char() != null) {
            simple_type.add("type_char", visitType_char(ctx.type_char()));
        } else if (ctx.type_float() != null) {
            simple_type.add("type_float", visitType_float(ctx.type_float()));
        }

        return simple_type;
    }

    @Override
    public ST visitType_fixed(SmallPearlParser.Type_fixedContext ctx) {
        ST fixed_type = group.getInstanceOf("fixed_type");
        Integer width = 31;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
            if (width < 1 || width > 63) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        fixed_type.add("size", width);

        return fixed_type;
    }

    @Override
    public ST visitType_char(SmallPearlParser.Type_charContext ctx) {
        ST char_type = group.getInstanceOf("char_type");
        Integer width = 1;

        if (ctx.IntegerConstant() != null) {
            width = Integer.parseInt(ctx.IntegerConstant().getText());
        }

        char_type.add("size", width);

        return char_type;
    }

    @Override
    public ST visitType_float(SmallPearlParser.Type_floatContext ctx) {
        ST float_type = group.getInstanceOf("float_type");
        return float_type;
    }

    @Override
    public ST visitTypeTime(SmallPearlParser.TypeTimeContext ctx) {
        ST time_type = group.getInstanceOf("time_type");

        if (ctx.type_clock() != null) {
            time_type.add("clock_type", visitType_clock(ctx.type_clock()));
        } else if (ctx.type_duration() != null) {
            time_type.add("duration_type", visitType_duration(ctx.type_duration()));
        }

        return time_type;
    }

    @Override
    public ST visitType_clock(SmallPearlParser.Type_clockContext ctx) {
        ST type = group.getInstanceOf("clock_type");

        type.add("init", 0);
        return type;
    }

    @Override
    public ST visitType_duration(SmallPearlParser.Type_durationContext ctx) {
        ST type = group.getInstanceOf("duration_type");
        type.add("init", 0);
        return type;
    }

    @Override
    public ST visitScalarVariableDeclaration(SmallPearlParser.ScalarVariableDeclarationContext ctx) {
        ST scalarVariableDeclaration = group.getInstanceOf("ScalarVariableDeclaration");

        if (ctx != null) {
            for (int i = 0; i < ctx.variableDenotation().size(); i++) {
                scalarVariableDeclaration.add("variable_denotations", visitVariableDenotation(ctx.variableDenotation().get(i)));
            }

            if (ctx.cpp_inline() != null) {
                scalarVariableDeclaration.add("cpp", visitCpp_inline(ctx.cpp_inline()));
            }
        }

        return scalarVariableDeclaration;
    }

    @Override
    public ST visitVariableDenotation(SmallPearlParser.VariableDenotationContext ctx) {
        ST variableDenotation = group.getInstanceOf("variable_denotation");
        ST typeAttribute = group.getInstanceOf("TypeAttribute");
        boolean hasGlobalAttribute = false;
        boolean hasAllocationProtection = false;

        ArrayList<String> identifierDenotationList = null;
        ArrayList<ST> initElementList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {

                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                } else if (c instanceof SmallPearlParser.AllocationProtectionContext) {
                    hasAllocationProtection = true;
                } else if (c instanceof SmallPearlParser.TypeAttributeContext) {
                    typeAttribute = visitTypeAttribute((SmallPearlParser.TypeAttributeContext) c);
                } else if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                    hasGlobalAttribute = true;
                } else if (c instanceof SmallPearlParser.InitialisationAttributeContext) {
                    initElementList = getInitialisationAttribute((SmallPearlParser.InitialisationAttributeContext) c);
                }
            }

            if (initElementList != null && identifierDenotationList.size() != initElementList.size()) {
                throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                ST v = group.getInstanceOf("VariableDeclaration");
                v.add("name", identifierDenotationList.get(i));
                v.add("TypeAttribute", typeAttribute);
                v.add("global", hasGlobalAttribute);
                v.add("inv", hasAllocationProtection);

                if (initElementList != null)
                    v.add("InitElement", initElementList.get(i));

                variableDenotation.add("decl", v);
            }
        }

        return variableDenotation;
    }

    private ArrayList<String> getIdentifierDenotation(SmallPearlParser.IdentifierDenotationContext ctx) {
        ArrayList<String> identifierDenotationList = new ArrayList();

        if (ctx != null) {
            for (int i = 0; i < ctx.ID().size(); i++) {
                identifierDenotationList.add(ctx.ID().get(i).toString());
            }
        }

        return identifierDenotationList;
    }

    private ArrayList<Integer> getPreset(SmallPearlParser.PresetContext ctx) {
        ArrayList<Integer> presetList = new ArrayList();

        if (ctx != null) {
            for (int i = 0; i < ctx.integerWithoutPrecision().size(); i++) {
                Integer preset = Integer.parseInt(ctx.integerWithoutPrecision(i).IntegerConstant().getText());
                presetList.add(preset);
            }
        }

        return presetList;
    }

    private ArrayList<ST> getInitialisationAttribute(SmallPearlParser.InitialisationAttributeContext ctx) {
        ArrayList<ST> initElementList = new ArrayList<ST>();

        if (ctx != null) {
            for (int i = 0; i < ctx.initElement().size(); i++) {
// // TODO: expression
                initElementList.add(getInitElement(ctx.initElement(i).constant()));
            }
        }

        return initElementList;
    }

    private ST getInitElement(SmallPearlParser.ConstantContext ctx) {
        ST constant = group.getInstanceOf("Constant");

        if (ctx != null) {
            if (ctx.IntegerConstant() != null) {
                ST integerConstant = group.getInstanceOf("IntegerConstant");
                Integer value;
                Integer sign = 1;


                value = Integer.parseInt(ctx.IntegerConstant().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }


                if (Integer.toBinaryString(Math.abs(value)).length() < 31) {
                    integerConstant.add("value", value.toString());
                } else {
                    integerConstant.add("value", value.toString() + "LL");
                }

                constant.add("IntegerConstant", integerConstant);
            } else if (ctx.durationConstant() != null) {
                ST durationConstant = group.getInstanceOf("DurationConstant");
                durationConstant.add("value", visitDurationConstant(ctx.durationConstant()));
                constant.add("DurationConstant", durationConstant);
            } else if (ctx.timeConstant() != null) {
                ST timeConstant = group.getInstanceOf("TimeConstant");
                timeConstant.add("value", visitTimeConstant(ctx.timeConstant()));
                constant.add("TimeConstant", timeConstant);
            } else if (ctx.FloatingPointConstant() != null) {
                ST durationConstant = group.getInstanceOf("FloatingPointConstant");
                Double value;
                Integer sign = 1;

                value = Double.parseDouble(ctx.FloatingPointConstant().getText());

                if (ctx.getChildCount() > 1) {
                    if (ctx.getChild(0).getText().equals("-")) {
                        value = -value;
                    }
                }

                constant.add("FloatingPointConstant", value);
            } else if (ctx.StringLiteral() != null) {
                ST stringConstant = group.getInstanceOf("StringConstant");
                String s = ctx.StringLiteral().toString();

                if(s.startsWith("'")) {
                    s = s.substring(1, s.length());
                }

                if(s.endsWith("'")) {
                    s = s.substring(0, s.length() - 1);
                }

                stringConstant.add("value", s);
                constant.add("StringConstant", stringConstant);
            }
        }

        return constant;
    }

    @Override
    public ST visitDurationConstant(SmallPearlParser.DurationConstantContext ctx) {
        ST st = group.getInstanceOf("DurationConstant");
        Integer hours = 0;
        Integer minutes = 0;
        Double seconds = 0.0;
        Double duration = 0.0;

        if (ctx.hours() != null) {
            hours = getHours(ctx.hours());
        }

        if (ctx.minutes() != null) {
            minutes = getMinutes(ctx.minutes());
        }

        if (ctx.seconds() != null) {
            seconds = getSeconds(ctx.seconds());
        }


        duration = hours * 3600 + minutes * 60 + seconds;

        st.add("value", duration);
        return st;
    }

    private Integer getHours(SmallPearlParser.HoursContext ctx) {
        Integer hours = 0;

        if (ctx.IntegerConstant() != null) {
            hours = Integer.parseInt(ctx.IntegerConstant().getText());
            if (hours < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return hours;
    }

    private Integer getMinutes(SmallPearlParser.MinutesContext ctx) {
        Integer minutes = 0;

        if (ctx.IntegerConstant() != null) {
            minutes = Integer.parseInt(ctx.IntegerConstant().getText());
            if (minutes < 0 || minutes > 59) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return minutes;
    }

    private Double getSeconds(SmallPearlParser.SecondsContext ctx) {
        Double seconds = 0.0;

        if (ctx.IntegerConstant() != null) {
            seconds = (double) Integer.parseInt(ctx.IntegerConstant().getText());
            if (seconds < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        } else if (ctx.FloatingPointConstant() != null) {
            Integer sign = 1;

            seconds = Double.parseDouble(ctx.FloatingPointConstant().getText());

            if (seconds < 0) {
                throw new ValueOutOfBoundsException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        return seconds;
    }

    @Override
    public ST visitTypeAttribute(SmallPearlParser.TypeAttributeContext ctx) {
        ST type = group.getInstanceOf("TypeAttribute");

        if (ctx.simpleType() != null) {
            type.add("SimpleType", visitSimpleType(ctx.simpleType()));
        }
        return type;
    }

    @Override
    public ST visitSimpleType(SmallPearlParser.SimpleTypeContext ctx) {
        ST simpleType = group.getInstanceOf("SimpleType");

        if (ctx != null) {
            if (ctx.typeInteger() != null) {
                simpleType.add("TypeInteger", visitTypeInteger(ctx.typeInteger()));
            } else if (ctx.typeDuration() != null) {
                simpleType.add("TypeDuration", visitTypeDuration(ctx.typeDuration()));
            } else if (ctx.typeBitString() != null) {
                simpleType.add("TypeBitString", visitTypeBitString(ctx.typeBitString()));
            } else if (ctx.typeFloatingPointNumber() != null) {
                simpleType.add("TypeFloatingPointNumber", visitTypeFloatingPointNumber(ctx.typeFloatingPointNumber()));
            } else if (ctx.typeTime() != null) {
                simpleType.add("TypeTime", visitTypeTime(ctx.typeTime()));
            } else if (ctx.typeCharacterString() != null) {
// TODO: TypeCharacterString
                simpleType.add("TypeCharacterString", visitTypeCharacterString(ctx.typeCharacterString()));
            }
        }

        return simpleType;
    }

    @Override
    public ST visitTypeDuration(SmallPearlParser.TypeDurationContext ctx) {
        ST st = group.getInstanceOf("TypeDuration");
        Integer size = 31;

        if (ctx != null) {
            st.add("code", 1);
        }

        return st;
    }

    @Override
    public ST visitTypeInteger(SmallPearlParser.TypeIntegerContext ctx) {
        ST st = group.getInstanceOf("TypeInteger");
        Integer size = 31;


        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.MprecisionContext) {
                    size = Integer.parseInt(((SmallPearlParser.MprecisionContext) c).integerWithoutPrecision().IntegerConstant().getText());
                }
            }
        }

        st.add("size", size);

        return st;
    }

    @Override
    public ST visitTypeBitString(SmallPearlParser.TypeBitStringContext ctx) {
        ST st = group.getInstanceOf("TypeBitString");

        Integer length = 1;

        if (ctx.IntegerConstant() != null) {
            length = Integer.parseInt(ctx.IntegerConstant().getText());
            if (length < 1 || length > 64) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        st.add("length", length);

        return st;

    }

    @Override
    public ST visitTypeCharacterString(SmallPearlParser.TypeCharacterStringContext ctx) {
        ST st = group.getInstanceOf("TypeCharacterString");
        Integer size = 1;


        if (ctx.IntegerConstant() != null) {
            size = Integer.parseInt(ctx.IntegerConstant().getText());

            if (size < 1 || size > 255) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        st.add("size", size);

        return st;
    }

    @Override
    public ST visitTypeFloatingPointNumber(SmallPearlParser.TypeFloatingPointNumberContext ctx) {
        ST st = group.getInstanceOf("TypeFloatingPointNumber");
        Integer precision = 53;


        if (ctx.IntegerConstant() != null) {
            precision = Integer.parseInt(ctx.IntegerConstant().getText());

            if (precision != 24 && precision != 53) {
                throw new NotSupportedTypeException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
            }
        }

        st.add("precision", precision);

        return st;
    }

    private ST variable_init(SmallPearlParser.Variable_initContext ctx) {
        ST vinit = group.getInstanceOf("variable_init");

        Integer value;

        if (ctx.constant() != null) {
            value = Integer.parseInt(ctx.constant().getText());

            if (Integer.toBinaryString(Math.abs(value)).length() < 31) {
                vinit.add("value", ctx.constant().getText());
            } else {
                vinit.add("value", ctx.constant().getText() + "LL");
            }
        }

        return vinit;
    }

    @Override
    public ST visitProblem_part(SmallPearlParser.Problem_partContext ctx) {
        ST problem_part = group.getInstanceOf("ProblemPart");

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    problem_part.add("ScalarVariableDeclarations", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.SemaDeclarationContext) {
                    problem_part.add("SemaDeclarations", visitSemaDeclaration((SmallPearlParser.SemaDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.TaskDeclarationContext) {
                    problem_part.add("TaskDeclarations", visitTaskDeclaration((SmallPearlParser.TaskDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.DationSpecificationContext) {
                    problem_part.add("DationSpecifications", visitDationSpecification((SmallPearlParser.DationSpecificationContext) c));
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    problem_part.add("DationDeclarations", visitDationDeclaration((SmallPearlParser.DationDeclarationContext) c));
                }
            }
        }

        SymbolTable symtab = SymbolTable.getSymbolTable();
        ST semaphoreArrays = group.getInstanceOf("TemporarySemaphoreArrays");

        LinkedList<TemporarySemaphoreArray> temporarySemaphoreArrays = symtab.getTemporarySemaphoreArrays();

        for (int i = 0; i < temporarySemaphoreArrays.size(); i++) {
            ST semaphoreArray = group.getInstanceOf("TemporarySemaphoreArray");
            semaphoreArray.add("name", temporarySemaphoreArrays.get(i).getArrayName());

            LinkedList<String> semaphores = temporarySemaphoreArrays.get(i).getSemaphores();
            for (int j = 0; j < semaphores.size(); j++) {
                semaphoreArray.add("semaphore", semaphores.get(j));
            }

            semaphoreArrays.add("array", semaphoreArray);
        }

        problem_part.add("temporarySemaphoreArrays", semaphoreArrays);

        return problem_part;
    }

    @Override
    public ST visitSemaDeclaration(SmallPearlParser.SemaDeclarationContext ctx) {
        ST st = group.getInstanceOf("SemaDeclaration");
        boolean hasGlobalAttribute = false;

        ArrayList<String> identifierDenotationList = null;
        ArrayList<Integer> presetList = null;

        if (ctx != null) {
            if (ctx.globalAttribute() != null) {
                hasGlobalAttribute = true;
            }

            if (ctx.identifierDenotation() != null) {
                identifierDenotationList = getIdentifierDenotation(ctx.identifierDenotation());
            }

            if (ctx.preset() != null) {
                presetList = getPreset(ctx.preset());

                if (identifierDenotationList.size() != presetList.size()) {
                    throw new NumberOfInitializerMismatchException(ctx.getText(), ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
        }

        for (int i = 0; i < identifierDenotationList.size(); i++) {
            ST v = group.getInstanceOf("sema_declaration");
            v.add("name", identifierDenotationList.get(i));
            v.add("global", hasGlobalAttribute);

            if (presetList != null)
                v.add("preset", presetList.get(i));

            st.add("decl", v);
        }

        return st;
    }

    @Override
    public ST visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx) {
        ST taskdecl = group.getInstanceOf("task_declaration");
        Integer priority = 255;
        Integer main = 0;

        if (ctx.priority() != null) {
            priority = Integer.parseInt(ctx.priority().IntegerConstant().getText());
        }

        if (ctx.task_main() != null) {
            main = 1;
        }

        taskdecl.add("name", ctx.ID());
        taskdecl.add("priority", priority);
        taskdecl.add("main", main);

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.TaskBodyContext) {
                    taskdecl.add("body", visitTaskBody((SmallPearlParser.TaskBodyContext) c));
                } else if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    taskdecl.add("cpp", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                }
            }
        }

        return taskdecl;
    }

    @Override
    public ST visitTaskBody(SmallPearlParser.TaskBodyContext ctx) {
        ST taskbody = group.getInstanceOf("task_body");

        if (ctx != null && ctx.children != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.ScalarVariableDeclarationContext) {
                    taskbody.add("decls", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext) c));
                } else if (c instanceof SmallPearlParser.StatementContext) {
                    taskbody.add("statements", visitStatement((SmallPearlParser.StatementContext) c));
                } else if (c instanceof SmallPearlParser.DationDeclarationContext) {
                    taskbody.add("decls", visitDationDeclaration((SmallPearlParser.DationDeclarationContext) c));
                }
            }
        }

        return taskbody;
    }

    @Override
    public ST visitTimeConstant(SmallPearlParser.TimeConstantContext ctx) {
        ST st = group.getInstanceOf("TimeConstant");
        st.add("value", getTime(ctx));
        return st;
    }

    private ST getExpression(SmallPearlParser.ExpressionContext ctx) {
        ST st = group.getInstanceOf("expression");

        if (ctx != null ) {
            if (ctx instanceof SmallPearlParser.BaseExpressionContext) {
                st.add("code", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.AdditiveExpressionContext) {
                st.add("code", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx)));
            } else if (ctx instanceof SmallPearlParser.MultiplicativeExpressionContext) {
                st.add("code", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
                st.add("code", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
                st.add("code", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx));
            } else if (ctx instanceof SmallPearlParser.ExponentiationExpressionContext) {
                st.add("code", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx));
            }
        }
        return st;
    }

    @Override
    public ST visitStartConditionAT(SmallPearlParser.StartConditionATContext ctx) {
        ST st = group.getInstanceOf("StartConditionAT");

/* TODO
        if (ctx.expression() instanceof SmallPearlParser.BaseExpressionContext) {
            st.add("rhs", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.AdditiveExpressionContext) {
            st.add("rhs", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.MultiplicativeExpressionContext) {
            st.add("rhs", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
            st.add("rhs", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
            st.add("rhs", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx.expression()));

//        } else if (ctx.expression() instanceof SmallPearlParser.RelationalExpressionContext) {
//            st.add("rhs", visitRelationalExpression((SmallPearlParser.RelationalExpressionContext) ctx.expression()));
        }
*/
        return st;
    }

    @Override
    public ST visitStartConditionAFTER(SmallPearlParser.StartConditionAFTERContext ctx) {
        ST st = group.getInstanceOf("StartConditionAFTER");
/* TODO
        if (ctx.expression() instanceof SmallPearlParser.BaseExpressionContext) {
            st.add("rhs", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.AdditiveExpressionContext) {
            st.add("rhs", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.MultiplicativeExpressionContext) {
            st.add("rhs", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
            st.add("rhs", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
            st.add("rhs", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx.expression()));
        }
*/
        return st;
    }

    @Override
    public ST visitStatement(SmallPearlParser.StatementContext ctx) {
        ST stmt = group.getInstanceOf("statement");

        stmt.add("srcFilename", m_sourceFileName);
        stmt.add("srcLine", ctx.start.getLine());
        stmt.add("srcColumn", ctx.start.getCharPositionInLine());

        if (ctx != null && ctx.children != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.Unlabeled_statementContext) {
                    stmt.add("code", visitUnlabeled_statement((SmallPearlParser.Unlabeled_statementContext) c));
                } else if (c instanceof SmallPearlParser.Block_statementContext) {
                    stmt.add("code", visitBlock_statement((SmallPearlParser.Block_statementContext) c));
                } else if (c instanceof SmallPearlParser.Cpp_inlineContext) {
                    stmt.add("cpp", visitCpp_inline((SmallPearlParser.Cpp_inlineContext) c));
                }
            }
        }

        return stmt;
    }

    @Override
    public ST visitUnlabeled_statement(SmallPearlParser.Unlabeled_statementContext ctx) {
        ST statement = group.getInstanceOf("statement");

        if (ctx.empty_statement() != null) {
            statement.add("code", visitEmpty_statement(ctx.empty_statement()));
        } else if (ctx.assignment_statement() != null) {
            statement.add("code", visitAssignment_statement(ctx.assignment_statement()));
        } else if (ctx.sequential_control_statement() != null) {
            statement.add("code", visitSequential_control_statement(ctx.sequential_control_statement()));
        } else if (ctx.realtime_statement() != null) {
            statement.add("code", visitRealtime_statement(ctx.realtime_statement()));
        } else if (ctx.io_statement() != null) {
            statement.add("code", visitIo_statement(ctx.io_statement()));
        } else if (ctx.callStatement() != null) {
            statement.add("code", visitCallStatement(ctx.callStatement()));
        } else if (ctx.returnStatement() != null) {
            statement.add("code", visitReturnStatement(ctx.returnStatement()));
        }

        return statement;
    }

    @Override
    public ST visitSequential_control_statement(SmallPearlParser.Sequential_control_statementContext ctx) {
        ST statement = group.getInstanceOf("statement");

        if (ctx.if_statement() != null) {
            statement.add("code", visitIf_statement(ctx.if_statement()));
        }

        return statement;
    }

    @Override
    public ST visitIf_statement(SmallPearlParser.If_statementContext ctx) {
        ST stmt = group.getInstanceOf("if_statement");

        if (ctx.expression() instanceof SmallPearlParser.BaseExpressionContext) {
            stmt.add("rhs", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.AdditiveExpressionContext) {
            stmt.add("rhs", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.MultiplicativeExpressionContext) {
            stmt.add("rhs", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
            stmt.add("rhs", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
            stmt.add("rhs", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.AtanExpressionContext) {
            stmt.add("rhs", visitAtanExpression((SmallPearlParser.AtanExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.CosExpressionContext) {
            stmt.add("rhs", visitCosExpression((SmallPearlParser.CosExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.ExpExpressionContext) {
            stmt.add("rhs", visitExpExpression((SmallPearlParser.ExpExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.LnExpressionContext) {
            stmt.add("rhs", visitLnExpression((SmallPearlParser.LnExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SinExpressionContext) {
            stmt.add("rhs", visitSinExpression((SmallPearlParser.SinExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SqrtExpressionContext) {
            stmt.add("rhs", visitSqrtExpression((SmallPearlParser.SqrtExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.TanExpressionContext) {
            stmt.add("rhs", visitTanExpression((SmallPearlParser.TanExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.TanhExpressionContext) {
            stmt.add("rhs", visitTanhExpression((SmallPearlParser.TanhExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.EqRelationalExpressionContext) {
            stmt.add("rhs", visitEqRelationalExpression((SmallPearlParser.EqRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.NeRelationalExpressionContext) {
            stmt.add("rhs", visitNeRelationalExpression((SmallPearlParser.NeRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.LtRelationalExpressionContext) {
            stmt.add("rhs", visitLtRelationalExpression((SmallPearlParser.LtRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.LeRelationalExpressionContext) {
            stmt.add("rhs", visitLeRelationalExpression((SmallPearlParser.LeRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.GtRelationalExpressionContext) {
            stmt.add("rhs", visitGtRelationalExpression((SmallPearlParser.GtRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.GeRelationalExpressionContext) {
            stmt.add("rhs", visitGeRelationalExpression((SmallPearlParser.GeRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.FitExpressionContext) {
            stmt.add("rhs", visitFitExpression((SmallPearlParser.FitExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.ExponentiationExpressionContext) {
            stmt.add("rhs", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.AbsExpressionContext) {
            stmt.add("rhs", visitAbsExpression((SmallPearlParser.AbsExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SizeofExpressionContext) {
            stmt.add("rhs", visitSizeofExpression((SmallPearlParser.SizeofExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.EntierExpressionContext) {
            stmt.add("rhs", visitEntierExpression((SmallPearlParser.EntierExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.RoundExpressionContext) {
            stmt.add("rhs", visitRoundExpression((SmallPearlParser.RoundExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SignExpressionContext) {
            stmt.add("rhs", visitSignExpression((SmallPearlParser.SignExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.RemainderExpressionContext) {
            stmt.add("rhs", visitRemainderExpression((SmallPearlParser.RemainderExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.NowFunctionContext) {
            stmt.add("rhs", visitNowFunction((SmallPearlParser.NowFunctionContext) ctx.expression()));
        }

        if (ctx.then_block() != null) {
            stmt.add("then_block", visitThen_block(ctx.then_block()));
        }

        if (ctx.else_block() != null) {
            stmt.add("else_block", visitElse_block(ctx.else_block()));
        }

        return stmt;
    }

    @Override
    public ST visitElse_block(SmallPearlParser.Else_blockContext ctx) {
        ST statement = group.getInstanceOf("statement");

        int i;
        for (i = 0; i < ctx.statement().size(); i++) {
            statement.add("code", visitStatement(ctx.statement(i)));
        }

        return statement;
    }

    @Override
    public ST visitThen_block(SmallPearlParser.Then_blockContext ctx) {
        ST statement = group.getInstanceOf("statement");

        int i;
        for (i = 0; i < ctx.statement().size(); i++) {
            statement.add("code", visitStatement(ctx.statement(i)));
        }

        return statement;
    }

    @Override
    public ST visitEmpty_statement(SmallPearlParser.Empty_statementContext ctx) {
        ST statement = group.getInstanceOf("empty_statement");
        return statement;
    }

    @Override
    public ST visitAssignment_statement(SmallPearlParser.Assignment_statementContext ctx) {
        ST stmt = group.getInstanceOf("assignment_statement");

        stmt.add("lhs", ctx.ID());

        if (ctx.expression() instanceof SmallPearlParser.BaseExpressionContext) {
            stmt.add("rhs", visitBaseExpression(((SmallPearlParser.BaseExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.AdditiveExpressionContext) {
            stmt.add("rhs", visitAdditiveExpression(((SmallPearlParser.AdditiveExpressionContext) ctx.expression())));
        } else if (ctx.expression() instanceof SmallPearlParser.MultiplicativeExpressionContext) {
            stmt.add("rhs", visitMultiplicativeExpression((SmallPearlParser.MultiplicativeExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnaryAdditiveExpressionContext) {
            stmt.add("rhs", visitUnaryAdditiveExpression((SmallPearlParser.UnaryAdditiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.UnarySubtractiveExpressionContext) {
            stmt.add("rhs", visitUnarySubtractiveExpression((SmallPearlParser.UnarySubtractiveExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.AtanExpressionContext) {
            stmt.add("rhs", visitAtanExpression((SmallPearlParser.AtanExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.CosExpressionContext) {
            stmt.add("rhs", visitCosExpression((SmallPearlParser.CosExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.ExpExpressionContext) {
            stmt.add("rhs", visitExpExpression((SmallPearlParser.ExpExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.LnExpressionContext) {
            stmt.add("rhs", visitLnExpression((SmallPearlParser.LnExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SinExpressionContext) {
            stmt.add("rhs", visitSinExpression((SmallPearlParser.SinExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SqrtExpressionContext) {
            stmt.add("rhs", visitSqrtExpression((SmallPearlParser.SqrtExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.TanExpressionContext) {
            stmt.add("rhs", visitTanExpression((SmallPearlParser.TanExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.TanhExpressionContext) {
            stmt.add("rhs", visitTanhExpression((SmallPearlParser.TanhExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.EqRelationalExpressionContext) {
            stmt.add("rhs", visitEqRelationalExpression((SmallPearlParser.EqRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.NeRelationalExpressionContext) {
            stmt.add("rhs", visitNeRelationalExpression((SmallPearlParser.NeRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.LtRelationalExpressionContext) {
            stmt.add("rhs", visitLtRelationalExpression((SmallPearlParser.LtRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.LeRelationalExpressionContext) {
            stmt.add("rhs", visitLeRelationalExpression((SmallPearlParser.LeRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.GtRelationalExpressionContext) {
            stmt.add("rhs", visitGtRelationalExpression((SmallPearlParser.GtRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.GeRelationalExpressionContext) {
            stmt.add("rhs", visitGeRelationalExpression((SmallPearlParser.GeRelationalExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.FitExpressionContext) {
            stmt.add("rhs", visitFitExpression((SmallPearlParser.FitExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.ExponentiationExpressionContext) {
            stmt.add("rhs", visitExponentiationExpression((SmallPearlParser.ExponentiationExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.AbsExpressionContext) {
            stmt.add("rhs", visitAbsExpression((SmallPearlParser.AbsExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SizeofExpressionContext) {
            stmt.add("rhs", visitSizeofExpression((SmallPearlParser.SizeofExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.EntierExpressionContext) {
            stmt.add("rhs", visitEntierExpression((SmallPearlParser.EntierExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.RoundExpressionContext) {
            stmt.add("rhs", visitRoundExpression((SmallPearlParser.RoundExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.SignExpressionContext) {
            stmt.add("rhs", visitSignExpression((SmallPearlParser.SignExpressionContext) ctx.expression()));
        } else if (ctx.expression() instanceof SmallPearlParser.RemainderExpressionContext) {
            stmt.add("rhs", visitRemainderExpression((SmallPearlParser.RemainderExpressionContext) ctx.expression()));
        }

        return stmt;
    }

    @Override
    public ST visitRealtime_statement(SmallPearlParser.Realtime_statementContext ctx) {
//        throw new NotYetImplementedException( "realtime_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());

        ST statement = group.getInstanceOf("statement");

        if (ctx.task_control_statement() != null) {
            statement.add("code", visitTask_control_statement(ctx.task_control_statement()));
        } else if (ctx.task_coordination_statement() != null) {
            statement.add("code", visitTask_coordination_statement(ctx.task_coordination_statement()));
        }

        return statement;

    }

    @Override
    public ST visitBaseExpression(SmallPearlParser.BaseExpressionContext ctx) {
        ST expression = group.getInstanceOf("expression");

        if (ctx.primaryExpression() != null) {
            expression.add("code", visitPrimaryExpression(ctx.primaryExpression()));
        }

//        System.out.println("visitBaseExpression: expression=" + expression.getAttributes().toString());

        return expression;

    }

    @Override
    public ST visitPrimaryExpression(SmallPearlParser.PrimaryExpressionContext ctx) {
        ST expression = group.getInstanceOf("expression");

        if (ctx.expression() != null) {
            expression.add("code", "(");
            expression.add("code", visit(ctx.expression()));
            expression.add("code", ")");
        } else if (ctx.literal() != null) {
            if (ctx.literal().BitStringLiteral() != null) {
                expression.add("bitstring", ctx.literal().BitStringLiteral().getText());
            } else {
                expression.add("code", visitLiteral(ctx.literal()));
            }
        } else if (ctx.ID() != null) {
            expression.add("id", getUserVariable(ctx.ID().getText()));
        } else if (ctx.semaTry() != null) {
            expression.add("code", visitSemaTry(ctx.semaTry()));
        }
//        else if (ctx.monadicArithmeticOperators() != null) {
//            expression.add("operator", getMonadiciArithmeticOperator(ctx.monadicArithmeticOperators()));
//        }

//        System.out.println("visitPrimaryExpression: expression=" + expression.getAttributes().toString());

        return expression;
    }


//    private ST getMonadiciArithmeticOperator(SmallPearlParser.MonadicArithmeticOperatorsContext ctx) {
//        ST st = group.getInstanceOf("MonadiciArithmeticOperator");
//        String operator = ctx.getChild(0).getText();
//
//        if (  operator.equals("SQRT")) {
//            operator = "sqrt";
//        } else if (  operator.equals("SIN")) {
//            operator = "sin";
//        } else if (  operator.equals("COS")) {
//            operator = "cos";
//        } else if (  operator.equals("EXP")) {
//            operator = "exp";
//        } else if (  operator.equals("LN")) {
//            operator = "ln";
//        } else if (  operator.equals("TAN")) {
//            operator = "tan";
//        } else if (  operator.equals("ATAN")) {
//            operator = "atan";
//        } else if (  operator.equals("TANH")) {
//            operator = "tanh";
//        }
//
//        st.add("operator", operator);
//        st.add("operand",  visit(ctx.getChild(1)));
//
//        return st;
//    }

    @Override
    public ST visitAdditiveExpression(SmallPearlParser.AdditiveExpressionContext ctx) {
        ST expr = group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitMultiplicativeExpression(SmallPearlParser.MultiplicativeExpressionContext ctx) {
        ST expr = group.getInstanceOf("expression");

        expr.add("code", visit(ctx.expression(0)));
        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitUnaryExpression(SmallPearlParser.UnaryExpressionContext ctx) {
        ST st = group.getInstanceOf("expression");

//        expr.add( "code", visit(ctx.expression(0)));
//        expr.add( "code", ctx.op.getText());
//        expr.add( "code", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitUnaryAdditiveExpression(SmallPearlParser.UnaryAdditiveExpressionContext ctx) {
        ST expr = group.getInstanceOf("expression");
        expr.add("code", visit(ctx.expression()));
        return expr;
    }

    @Override
    public ST visitUnarySubtractiveExpression(SmallPearlParser.UnarySubtractiveExpressionContext ctx) {
        ST expr = group.getInstanceOf("expression");

        expr.add("code", ctx.op.getText());
        expr.add("code", visit(ctx.expression()));
        return expr;
    }

    @Override
    public ST visitExponentiationExpression(SmallPearlParser.ExponentiationExpressionContext ctx) {
        ST expr = group.getInstanceOf("Exponentiation");

        expr.add("lhs", visit(ctx.expression(0)));
        expr.add("rhs", visit(ctx.expression(1)));

        return expr;
    }

    @Override
    public ST visitLiteral(SmallPearlParser.LiteralContext ctx) {
        ST literal = group.getInstanceOf("literal");

        if (ctx.IntegerConstant() != null) {
            literal.add("integer", ctx.IntegerConstant().getText());
        } else if (ctx.StringLiteral() != null) {
            String s = ctx.StringLiteral().getText();

            if(s.startsWith("'")) {
                s = s.substring(1, s.length());
            }

            if(s.endsWith("'")) {
                s = s.substring(0, s.length() - 1);
            }

            literal.add("string", s);
        } else if (ctx.FloatingPointConstant() != null) {
            literal.add("float", Double.valueOf(ctx.FloatingPointConstant().toString()));
        } else if (ctx.timeConstant() != null) {
            literal.add("time", getTime(ctx.timeConstant()));
        } else if (ctx.durationConstant() != null) {
            literal.add("duration", getDuration(ctx.durationConstant()));
        }

        return literal;
    }

    @Override
    public ST visitReturnStatement(SmallPearlParser.ReturnStatementContext ctx) {
        ST stmt = group.getInstanceOf("return_statement");
//TODO:         stmt.add("expression", visitExpression(ctx.expression()));
        return stmt;
    }

    @Override
    public ST visitTask_control_statement(SmallPearlParser.Task_control_statementContext ctx) {
        ST stmt = group.getInstanceOf("task_control_statement");

        if (ctx.taskStart() != null) {
            stmt.add("code", visitTaskStart(ctx.taskStart()));
        } else if (ctx.task_terminating() != null) {
            stmt.add("code", visitTask_terminating(ctx.task_terminating()));
        } else if (ctx.task_suspending() != null) {
            stmt.add("code", visitTask_suspending(ctx.task_suspending()));
        } else if (ctx.taskContinuation() != null) {
            stmt.add("code", visitTaskContinuation(ctx.taskContinuation()));
        } else if (ctx.taskResume() != null) {
            stmt.add("code", visitTaskResume(ctx.taskResume()));
        } else if (ctx.task_preventing() != null) {
            stmt.add("code", visitTask_preventing(ctx.task_preventing()));
        }

        return stmt;
    }

    private ST getTaskStartConditions(SmallPearlParser.TaskStartContext ctx) {
        ST st = group.getInstanceOf("TaskStartConditions");

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            st.add("Condition", "AT");
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            st.add("Condition", "AFTER");
        }

        if (ctx.frequency() != null) {
            SmallPearlParser.FrequencyContext c = ctx.frequency();
            st.add("Condition", "ALL");

            for (int i = 0; i < c.getChildCount(); i++) {
                if (c.getChild(i) instanceof TerminalNodeImpl) {
                    if (((TerminalNodeImpl) c.getChild(i)).getSymbol().getText().equals("UNTIL")) {
                        st.add("Condition", "UNTIL");
                    } else if (((TerminalNodeImpl) c.getChild(i)).getSymbol().getText().equals("DURING")) {
                        st.add("Condition", "DURING");
                    }
                }
            }
        }

        if (ctx.priority() != null) {
            st.add("Condition", "PRIO");
        }

        return st;
    }

    @Override
    public ST visitTaskStart(SmallPearlParser.TaskStartContext ctx) {
        ST st = group.getInstanceOf("task_start");

        st.add("task", ctx.ID().toString());

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            SmallPearlParser.StartConditionATContext c = (SmallPearlParser.StartConditionATContext) ctx.startCondition();
            st.add("Condition", "AT");
            if (c.timeConstant() != null) {
                st.add("at", getTime(c.timeConstant()));
            }
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            SmallPearlParser.StartConditionAFTERContext c = (SmallPearlParser.StartConditionAFTERContext) ctx.startCondition();
            st.add("Condition", "AFTER");
            if (c.durationConstant() != null) {
                st.add("after", getDuration(c.durationConstant()).toString());
            }
        }

        if (ctx.frequency() != null) {
            SmallPearlParser.FrequencyContext c = ctx.frequency();
            st.add("Condition", "ALL");
            st.add("all", getDuration(c.durationConstant().get(0)).toString());

            for (int i = 0; i < c.getChildCount(); i++) {
                if (c.getChild(i) instanceof TerminalNodeImpl) {
                    if (((TerminalNodeImpl) c.getChild(i)).getSymbol().getText().equals("UNTIL")) {
                        st.add("Condition", "UNTIL");
                        st.add("until", getTime(c.timeConstant()).toString());
                    } else if (((TerminalNodeImpl) c.getChild(i)).getSymbol().getText().equals("DURING")) {
                        st.add("Condition", "DURING");
                        st.add("during", getDuration(c.durationConstant().get(1)).toString());
                    }
                }
            }
        }

        if (ctx.priority() != null) {
            st.add("Condition", "PRIO");
            st.add("priority", Integer.parseInt(ctx.priority().IntegerConstant().getText()));
        }

        return st;
    }

    @Override
    public ST visitTask_terminating(SmallPearlParser.Task_terminatingContext ctx) {
        ST stmt = group.getInstanceOf("task_terminate");

        if (ctx.ID() != null) {
            stmt.add("task", ctx.ID().toString());
        }

        return stmt;
    }

    @Override
    public ST visitTask_suspending(SmallPearlParser.Task_suspendingContext ctx) {
        ST stmt = group.getInstanceOf("task_suspend");

        if (ctx.ID() != null) {
            stmt.add("task", ctx.ID().toString());
        }

        return stmt;
    }

    @Override
    public ST visitTaskContinuation(SmallPearlParser.TaskContinuationContext ctx) {
        ST st = group.getInstanceOf("TaskContinuation");
        st.add("task", ctx.ID().toString());

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            SmallPearlParser.StartConditionATContext c = (SmallPearlParser.StartConditionATContext) ctx.startCondition();
            st.add("Condition", "AT");
            if (c.timeConstant() != null) {
                st.add("at", getTime(c.timeConstant()));
            }
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            SmallPearlParser.StartConditionAFTERContext c = (SmallPearlParser.StartConditionAFTERContext) ctx.startCondition();
            st.add("Condition", "AFTER");
            if (c.durationConstant() != null) {
                st.add("after", getDuration(c.durationConstant()).toString());
            }
        }

        if (ctx.priority() != null) {
            st.add("Condition", "PRIO");
            st.add("priority", Integer.parseInt(ctx.priority().IntegerConstant().getText()));
        }

        return st;
    }

    @Override
    public ST visitTaskResume(SmallPearlParser.TaskResumeContext ctx) {
        ST st = group.getInstanceOf("TaskResume");

        if (ctx.startCondition() instanceof SmallPearlParser.StartConditionATContext) {
            SmallPearlParser.StartConditionATContext c = (SmallPearlParser.StartConditionATContext) ctx.startCondition();
            st.add("Condition", "AT");
            if (c.timeConstant() != null) {
                st.add("at", getTime(c.timeConstant()).toString());
            }
        } else if (ctx.startCondition() instanceof SmallPearlParser.StartConditionAFTERContext) {
            SmallPearlParser.StartConditionAFTERContext c = (SmallPearlParser.StartConditionAFTERContext) ctx.startCondition();
            st.add("Condition", "AFTER");
            if (c.durationConstant() != null) {
                st.add("after", getDuration(c.durationConstant()).toString());
            }
        }

        return st;
    }

    @Override
    public ST visitTask_preventing(SmallPearlParser.Task_preventingContext ctx) {
        ST stmt = group.getInstanceOf("task_prevent");
        stmt.add("task", ctx.ID().toString());
        return stmt;
    }

    @Override
    public ST visitTask_coordination_statement(SmallPearlParser.Task_coordination_statementContext ctx) {
        ST stmt = group.getInstanceOf("TaskCoordinationStatement");

        if (ctx.semaRelease() != null) {
            stmt.add("statement", visitSemaRelease(ctx.semaRelease()));
        } else if (ctx.semaRequest() != null) {
            stmt.add("statement", visitSemaRequest(ctx.semaRequest()));
        }
        return stmt;
    }

    @Override
    public ST visitSemaTry(SmallPearlParser.SemaTryContext ctx) {
        ST st = group.getInstanceOf("SemaTry");

        for (int i = 0; i < ctx.ID().size(); i++) {
            st.add("names", ctx.ID(i));
        }

        st.add("noofsemas", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitSemaRelease(SmallPearlParser.SemaReleaseContext ctx) {
        ST st = group.getInstanceOf("SemaRelease");

        for (int i = 0; i < ctx.ID().size(); i++) {
            st.add("names", ctx.ID(i));
        }

        st.add("noofsemas", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitSemaRequest(SmallPearlParser.SemaRequestContext ctx) {
        ST st = group.getInstanceOf("SemaRequest");

        for (int i = 0; i < ctx.ID().size(); i++) {
            st.add("names", ctx.ID(i));
        }

        st.add("noofsemas", ctx.ID().size());

        return st;
    }

    @Override
    public ST visitConstant(SmallPearlParser.ConstantContext ctx) {
        ST st = group.getInstanceOf("Constant");
        return st;
    }

    @Override
    public ST visitIo_statement(SmallPearlParser.Io_statementContext ctx) {
        ST stmt = group.getInstanceOf("io_statement");

        if (ctx.close_statement() != null) {
            stmt.add("code", visitClose_statement(ctx.close_statement()));
        } else if (ctx.open_statement() != null) {
            stmt.add("code", visitOpen_statement(ctx.open_statement()));
        } else if (ctx.readStatement() != null) {
            stmt.add("code", visitReadStatement(ctx.readStatement()));
        } else if (ctx.sendStatement() != null) {
            stmt.add("code", visitSendStatement(ctx.sendStatement()));
        } else if (ctx.takeStatement() != null) {
            stmt.add("code", visitTakeStatement(ctx.takeStatement()));
        } else if (ctx.writeStatement() != null) {
            stmt.add("code", visitWriteStatement(ctx.writeStatement()));
        } else if (ctx.getStatement() != null) {
            stmt.add("code", visitGetStatement(ctx.getStatement()));
        } else if (ctx.putStatement() != null) {
            stmt.add("code", visitPutStatement(ctx.putStatement()));
        }

        return stmt;
    }

    @Override
    public ST visitClose_statement(SmallPearlParser.Close_statementContext ctx) {
        ST stmt = group.getInstanceOf("close_statement");
        stmt.add("id", ctx.ID());

        if (ctx.close_parameterlist() != null) {
            stmt.add("paramlist", visitClose_parameterlist(ctx.close_parameterlist()));

            ArrayList<String> rstVars = getCloseRstVariables(ctx.close_parameterlist());

            if (rstVars.size() > 1) {
                throw new NotSupportedFeatureException("close_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "CLOSE: Mulitple RST not supported");
            }

            if (rstVars.size() == 1) {
                String var = rstVars.get(0).toString();

                stmt.add("rst_var", var);
            }
        }
        else {
            ST st = group.getInstanceOf("close_parameterlist");
            st.add("parameter", group.getInstanceOf("close_parameter_none"));
            stmt.add("paramlist", st);
            stmt.add("rst_var", group.getInstanceOf("close_parameter_no_rst"));
        }

        return stmt;
    }

    private boolean isRSTSpecified(SmallPearlParser.Close_parameterlistContext ctx) {
        boolean rstFound = false;

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Close_parameter_rstContext) {
                rstFound = true;
                break;
            }
        }

        return rstFound;
    }


/*
    OpenStatement ::=
    OPEN Name§Dation [ BY OpenParameter [ , OpenParameter ] ... ] ;

    When executing the open statement, a data station with typology is positioned at its beginning.
    The open parameters serve to handle data stations containing identifyable files. E.g., a system defined data station Disk can possess a file TAB1, which is also maintained after terminating the program under this name. Later on, the same or another program can create a user defined data station Table on Disk, identified with file TAB1 in the open statement.

    OpenParameter ::=
    IDF ( {Name§CharacterVariable | CharacterStringConstant } ) | RST (Name§ErrorVariable-FIXED) |
    { OLD | NEW | ANY } |
    { CAN | PRM
*/

    @Override
    public ST visitOpen_statement(SmallPearlParser.Open_statementContext ctx) {
        ST stmt = group.getInstanceOf("open_statement");

        stmt.add("id", ctx.ID());
        if (ctx.open_parameterlist() != null) {
            stmt.add("paramlist", visitOpen_parameterlist(ctx.open_parameterlist()));

            ArrayList<String> idfFilenames = getIdfFilenames(ctx.open_parameterlist());

            if (idfFilenames.size() > 1) {
                throw new NotSupportedFeatureException("open_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "OPEN: Mulitple IDF not supported");
            }

            if (idfFilenames.size() == 1) {
                String fname = idfFilenames.get(0).toString();
                fname = fname.substring(1, fname.length() - 1);

                stmt.add("fname", fname);
                stmt.add("flen", fname.length());
            }

            ArrayList<String> rstVars = getOpenRstVariables(ctx.open_parameterlist());

            if (rstVars.size() > 1) {
                throw new NotSupportedFeatureException("open_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine(), "OPEN: Mulitple RST not supported");
            }

            if (rstVars.size() == 1) {
                String var = rstVars.get(0).toString();

                stmt.add("rst_var", var);
            }
        }

        return stmt;
    }

    private ArrayList<String> getIdfFilenames(SmallPearlParser.Open_parameterlistContext ctx) {
        ArrayList<String> filenames = new ArrayList();

        if (ctx != null) {
            for (int i = 0; i < ctx.open_parameter().size(); i++) {
                if (ctx.open_parameter(i) instanceof SmallPearlParser.Open_parameter_idfContext) {
                    SmallPearlParser.Open_parameter_idfContext c = (SmallPearlParser.Open_parameter_idfContext) ctx.open_parameter(i);

                    if (c.StringLiteral() != null) {
                        filenames.add(c.StringLiteral().toString());
                    } else if (c.ID() != null) {
                        filenames.add(c.ID().toString());
                    }
                }

            }
        }

        return filenames;
    }

    private ArrayList<String> getOpenRstVariables(SmallPearlParser.Open_parameterlistContext ctx) {
        ArrayList<String> vars = new ArrayList();

        if (ctx != null) {
            for (int i = 0; i < ctx.open_parameter().size(); i++) {
                if (ctx.open_parameter(i) instanceof SmallPearlParser.Open_parameter_rstContext) {
                    SmallPearlParser.Open_parameter_rstContext c = (SmallPearlParser.Open_parameter_rstContext) ctx.open_parameter(i);

                    if (c.ID() != null) {
                        vars.add(c.ID().toString());
                    }
                }

            }
        }

        return vars;
    }

    private ArrayList<String> getCloseRstVariables(SmallPearlParser.Close_parameterlistContext ctx) {
        ArrayList<String> vars = new ArrayList();

        if (ctx != null) {
            for (int i = 0; i < ctx.close_parameter().size(); i++) {
                if (ctx.close_parameter(i) instanceof SmallPearlParser.Close_parameter_rstContext) {
                    SmallPearlParser.Close_parameter_rstContext c = (SmallPearlParser.Close_parameter_rstContext) ctx.close_parameter(i);

                    if (c.ID() != null) {
                        vars.add(c.ID().toString());
                    }
                }
            }
        }

        return vars;
    }

    @Override
    public ST visitOpen_parameterlist(SmallPearlParser.Open_parameterlistContext ctx) {
        ST st = group.getInstanceOf("open_parameterlist");

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Open_parameter_old_or_new_or_anyContext) {
                SmallPearlParser.Open_parameter_old_or_new_or_anyContext ctxTmp = (SmallPearlParser.Open_parameter_old_or_new_or_anyContext) c;
                if (ctxTmp.open_parameter_old_new_any() != null) {
                    st.add("parameter", visitOpen_parameter_old_or_new_or_any(ctxTmp));
                }
            } else if (c instanceof SmallPearlParser.Open_parameter_can_or_prmContext) {
                SmallPearlParser.Open_parameter_can_or_prmContext ctxTmp = (SmallPearlParser.Open_parameter_can_or_prmContext) c;
                if (ctxTmp.open_parameter_can_prm() != null) {
                    st.add("parameter", visitOpen_parameter_can_or_prm(ctxTmp));
                }
            } else if (c instanceof SmallPearlParser.Open_parameter_rstContext) {
                SmallPearlParser.Open_parameter_rstContext ctxTmp = (SmallPearlParser.Open_parameter_rstContext) c;
                ST stTmp = group.getInstanceOf("open_parameter_rst");

                if (ctxTmp.ID() != null) {
                    stTmp.add("id", ctxTmp.ID().getText());
                }

                st.add("parameter", stTmp);
            } else if (c instanceof SmallPearlParser.Open_parameter_idfContext) {
                SmallPearlParser.Open_parameter_idfContext ctxTmp = (SmallPearlParser.Open_parameter_idfContext) c;
                st.add("parameter", visitOpen_parameter_idf(ctxTmp));
            }

        }

        return st;
    }

    @Override
    public ST visitOpen_parameter_idf(SmallPearlParser.Open_parameter_idfContext ctx) {
        ST st = group.getInstanceOf("open_parameter_idf");

        if (ctx.ID() != null) {
            st.add("id", ctx.ID().getText());
        } else if (ctx.StringLiteral() != null) {
            st.add("string", ctx.StringLiteral().getText());
        }
        return st;
    }

    @Override
    public ST visitOpen_parameter_old_or_new_or_any(SmallPearlParser.Open_parameter_old_or_new_or_anyContext ctx) {
        if (ctx.open_parameter_old_new_any() instanceof SmallPearlParser.Open_parameter_oldContext) {
            ST st = group.getInstanceOf("open_parameter_old");
            st.add("attribute", 1);
            return st;
        } else if (ctx.open_parameter_old_new_any() instanceof SmallPearlParser.Open_parameter_newContext) {
            ST st = group.getInstanceOf("open_parameter_new");
            st.add("attribute", 1);
            return st;
        } else if (ctx.open_parameter_old_new_any() instanceof SmallPearlParser.Open_parameter_anyContext) {
            ST st = group.getInstanceOf("open_parameter_any");
            st.add("attribute", 1);
            return st;
        }

        return null;
    }


    @Override
    public ST visitClose_parameterlist(SmallPearlParser.Close_parameterlistContext ctx) {
        ST st = group.getInstanceOf("close_parameterlist");

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Close_parameter_canContext) {
                ST can = group.getInstanceOf("close_parameter_can");
                can.add("attribute", 1);
                st.add("parameter", can);
            } else if (c instanceof SmallPearlParser.Close_parameter_prmContext) {
                ST prm = group.getInstanceOf("close_parameter_prm");
                prm.add("attribute", 1);
                st.add("parameter", prm);
            } else if (c instanceof SmallPearlParser.Close_parameter_rstContext) {
                ST rst = group.getInstanceOf("close_parameter_rst");
                rst.add("id", ((SmallPearlParser.Close_parameter_rstContext) c).ID());
                st.add("parameter", rst);
            }
        }

        return st;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // GetStatement ::=
    //   GET [ { Name§Variable | Segment } [ , { Name§Variable | Segment } ] ... ] FROM Name§Dation [ BY FormatPosition [ , FormatPosition ] ... ] ;
    ////////////////////////////////////////////////////////////////////////////////
/*
    getStatement :
            'GET' ( ID ( ',' ID )* )? getStatementFrom getStatementBy? ';'
    ;

    getStatementFrom :
            'FROM' ID
    ;

    getStatementBy :
            'BY' formatPosition ( ',' formatPosition )*
    ;
*/

    @Override
    public ST visitGetStatement(SmallPearlParser.GetStatementContext ctx) {
        ST stmt = group.getInstanceOf("get_statement");

        String dation = ctx.getFrom().ID().getText();

        stmt.add("dation", getUserVariable(dation));

        int j = 0;
        for (int i = 0; i < ctx.ID().size(); i++) {
            if (ctx.formatPosition() != null) {
                int k = 0;
                boolean foundFormat = false;
                while ( k < ctx.formatPosition().size() && !foundFormat) {
                    if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatContext) {
                        ST e = getFactorFormatForGet((SmallPearlParser.FactorFormatContext) ctx.formatPosition(j), dation, ctx.ID(i).getText());
                        stmt.add("elements", e);
                        foundFormat = true;
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorPositionContext) {
                        // TODO: visitExpression???
                        ST e = getFactorPositionForGet((SmallPearlParser.FactorPositionContext) ctx.formatPosition(j), dation, ctx.ID(i).getText());
                        stmt.add("elements", e);
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatPositionContext) {
                    }

                    k = k + 1;
                    j = j + 1;
                    if (j == ctx.formatPosition().size()) {
                        j = 0;
                    }
                }

                if ( !foundFormat) {
                    throw new NoOfFormatSpecifiersDoesNotMatchException("get_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }
        }

        if (ctx.formatPosition() != null) {
            for(int k = j; k < ctx.formatPosition().size(); k++) {
                if (ctx.formatPosition(k) instanceof SmallPearlParser.FactorPositionContext) {
                    ST e = getFactorPositionForGet((SmallPearlParser.FactorPositionContext) ctx.formatPosition(k), dation, null);
                    stmt.add("elements", e);
                }
            }
        }

        return stmt;
    }

    @Override
    public ST visitPutStatement(SmallPearlParser.PutStatementContext ctx) {
        ST stmt = group.getInstanceOf("put_statement");
        stmt.add("dation", getUserVariable(ctx.ID().toString()));

        int j = 0;

        int formatCount = 0;
        for ( int i = 0; i < ctx.formatPosition().size(); i++ ) {
            if (ctx.formatPosition(i) instanceof SmallPearlParser.FactorFormatContext) {
                formatCount++;
            }
        }

        if ( formatCount > ctx.expression().size() ) {
            throw new NoOfFormatSpecifiersDoesNotMatchException("put_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        for (int i = 0; i < ctx.expression().size(); i++) {
            if (ctx.formatPosition() != null) {
                int k = 0;
                boolean foundFormat = false;

                while ( k < ctx.formatPosition().size() && !foundFormat) {
                    if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatContext) {
                        ST e = getFactorFormatForPut((SmallPearlParser.FactorFormatContext) ctx.formatPosition(j), ctx.ID().getText(), ctx.expression(i).getText());
                        stmt.add("elements", e);
                        foundFormat = true;
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorPositionContext) {
                        // TODO: visitExpression???
                        ST e = getFactorPositionForPut((SmallPearlParser.FactorPositionContext) ctx.formatPosition(j), ctx.ID().getText(), ctx.expression(i).getText());
                        stmt.add("elements", e);
                    } else if (ctx.formatPosition(j) instanceof SmallPearlParser.FactorFormatPositionContext) {
                    }

                    k = k + 1;
                    j = j + 1;
                    if (j == ctx.formatPosition().size()) {
                        j = 0;
                    }
                }

                if ( !foundFormat) {
                    throw new NoOfFormatSpecifiersDoesNotMatchException("put_statement", ctx.start.getLine(), ctx.start.getCharPositionInLine());
                }
            }

        }

        if (ctx.formatPosition() != null) {
            for(int k = j; k < ctx.formatPosition().size(); k++) {
                if (ctx.formatPosition(k) instanceof SmallPearlParser.FactorPositionContext) {
                    ST e = getFactorPositionForPut((SmallPearlParser.FactorPositionContext) ctx.formatPosition(k), ctx.ID().getText(), null);
                    stmt.add("elements", e);
                }
            }
        }

        return stmt;
    }

    private ST getFactorFormatForPut(SmallPearlParser.FactorFormatContext ctx, String dation, String element) {
        ST st = group.getInstanceOf("put_statement_factor_format");

        st.add("dation", getUserVariable(dation));

        if ( ctx.format().fixedFormat() != null ) {
            st.add("format", getFixedFormatForPut(ctx.format().fixedFormat(), element));
        }
        else {
            st.add("format", getCharacterStringFormatForPut(ctx.format().characterStringFormat(), element));
        }

        return st;
    }

    private ST getFactorFormatForGet(SmallPearlParser.FactorFormatContext ctx, String dation, String element) {
        ST st = group.getInstanceOf("get_statement_factor_format");

        st.add("dation", getUserVariable(dation));

        if ( ctx.format().fixedFormat() != null ) {
            st.add("format", getFixedFormatForGet(ctx.format().fixedFormat(), element));
        }
        else {
            st.add("format", getCharacterStringFormatForGet(ctx.format().characterStringFormat(), element));
        }

        return st;
    }

    private ST getCharacterStringFormatForPut(SmallPearlParser.CharacterStringFormatContext ctx, String element) {
        ST st = group.getInstanceOf("put_statement_character_string_format");

        st.add("element", getUserVariable(element));

        if( ctx instanceof SmallPearlParser.CharacterStringFormatAContext ) {
            SmallPearlParser.CharacterStringFormatAContext c = (SmallPearlParser.CharacterStringFormatAContext) ctx;

            if ( c.expression() != null ) {
                st.add("fieldwidth", getExpression(c.expression()));
            }
        }

        return st;
    }

    private ST getCharacterStringFormatForGet(SmallPearlParser.CharacterStringFormatContext ctx, String element) {
        ST st = group.getInstanceOf("get_statement_character_string_format");

        st.add("element", getUserVariable(element));

        if( ctx instanceof SmallPearlParser.CharacterStringFormatAContext ) {
            SmallPearlParser.CharacterStringFormatAContext c = (SmallPearlParser.CharacterStringFormatAContext) ctx;

            if ( c.expression() != null ) {
                st.add("fieldwidth", getExpression(c.expression()));
            }
        }

        return st;
    }

    private ST getFixedFormatForPut(SmallPearlParser.FixedFormatContext ctx, String element) {
        ST st = group.getInstanceOf("put_statement_fixed_format");

        st.add("element", getUserVariable(element));
        st.add( "fieldwidth",  getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));

            if ( ctx.scaleFactor() != null ) {
                st.add( "scaleFactor", getExpression(ctx.scaleFactor().expression()));
            }
        }

        return st;
    }

    private ST getFixedFormatForGet(SmallPearlParser.FixedFormatContext ctx, String element) {
        ST st = group.getInstanceOf("get_statement_fixed_format");

        st.add("element", getUserVariable(element));
        st.add( "fieldwidth", getExpression(ctx.fieldWidth().expression()));

        if ( ctx.decimalPositions() != null ) {
            st.add( "decimalPositions", getExpression(ctx.decimalPositions().expression()));

            if ( ctx.scaleFactor() != null ) {
                st.add( "scaleFactor", getExpression(ctx.scaleFactor().expression()));
            }
        }

        return st;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Position ::=
    //   RST ( Name§ErrorVariable-FIXED )
    //   | { X | SKIP | PAGE } [ ( Expression ) ]
    //   | { POS | ADV } ( Expression [ , Expression [ , Expression ] ] )
    //   | { COL | LINE } ( Expression )
    //   | SOP ( Name [ , Name [ , Name ] ] /∗ PositionVariables-FIXED ∗/ )
    ////////////////////////////////////////////////////////////////////////////////
    /*
    position :
      'RST' ( '(' ID ')' )               # positionRST
    | 'SKIP' ( '(' expression ')' )?     # positionSKIP
    | 'X' ( '(' expression ')' )?        # positionX
    ;

     */

    private ST getFactorPositionForPut(SmallPearlParser.FactorPositionContext ctx, String dation, String element) {
        ST st = group.getInstanceOf("put_statement_factor_position");
        st.add("dation", getUserVariable(dation));

        if ( ctx.position() instanceof  SmallPearlParser.PositionRSTContext ) {
            SmallPearlParser.PositionRSTContext c = (SmallPearlParser.PositionRSTContext) ctx.position();
            ST e = group.getInstanceOf("put_statement_factor_position_rst");
            e.add("element", getUserVariable(c.ID().getText()));
            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionSKIPContext ) {
            SmallPearlParser.PositionSKIPContext c = (SmallPearlParser.PositionSKIPContext) ctx.position();
            ST e = group.getInstanceOf("put_statement_factor_position_skip");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionXContext ) {
            SmallPearlParser.PositionXContext c = (SmallPearlParser.PositionXContext) ctx.position();
            ST e = group.getInstanceOf("put_statement_factor_position_x");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }

        return st;
    }

    private ST getFactorPositionForGet(SmallPearlParser.FactorPositionContext ctx, String dation, String element) {
        ST st = group.getInstanceOf("get_statement_factor_position");
        st.add("dation", getUserVariable(dation));

        if ( ctx.position() instanceof  SmallPearlParser.PositionRSTContext ) {
            SmallPearlParser.PositionRSTContext c = (SmallPearlParser.PositionRSTContext) ctx.position();
            ST e = group.getInstanceOf("get_statement_factor_position_rst");
            e.add("element", getUserVariable(c.ID().getText()));
            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionSKIPContext ) {
            SmallPearlParser.PositionSKIPContext c = (SmallPearlParser.PositionSKIPContext) ctx.position();
            ST e = group.getInstanceOf("get_statement_factor_position_skip");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }
        else if ( ctx.position() instanceof  SmallPearlParser.PositionXContext ) {
            SmallPearlParser.PositionXContext c = (SmallPearlParser.PositionXContext) ctx.position();
            ST e = group.getInstanceOf("get_statement_factor_position_x");

            if ( c.expression() != null ) {
                e.add("element", getExpression(c.expression()));
            }
            else {
                e.add("element", 1);
            }

            st.add("format", e);
        }

        return st;
    }

    private ST getUserVariable(String user_variable) {
        ST st = group.getInstanceOf("user_variable");
        st.add("name", user_variable);
        return st;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // SendStatement ::=
    //   SEND [ Expression ] TO Name§Dation
    //    [ BY RST-S-CTRL-Format [ , RST-S-CTRL-Format ] ... ] ;
    ////////////////////////////////////////////////////////////////////////////////

    @Override
    public ST visitSendStatement(SmallPearlParser.SendStatementContext ctx) {
        ST st = group.getInstanceOf("send_statement");
        String dation = "";

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.SendToContext) {
                dation = ((SmallPearlParser.SendToContext)c).ID().getText();
            }
        }

        st.add("dation", getUserVariable(dation));

        for (int i = 0; i < ctx.take_send_rst_s_ctrl_format().size(); i++) {
            if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_RST(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_SContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_S(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_CONTROL(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            }
        }

        if ( ctx.expression() != null ) {
            ST el = group.getInstanceOf("send_expression");
            el.add("expression", getUserVariable((ctx.expression().getText())));
            el.add("dation", getUserVariable(dation));
            st.add("elements", el);
        }

        return st;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // TakeStatement ::=
    //   TAKE [ Name§Variable ] FROM Name§Dation
    //     [ BY RST-S-CTRL-Format [ , RST-S-CTRL-Format ] ... ] ;
    ////////////////////////////////////////////////////////////////////////////////

    @Override
    public ST visitTakeStatement(SmallPearlParser.TakeStatementContext ctx) {
        ST st = group.getInstanceOf("take_statement");
        String dation = "";

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.TakeFromContext) {
                dation = ((SmallPearlParser.TakeFromContext)c).ID().getText();
            }
        }

        st.add("dation", getUserVariable(dation));

        for (int i = 0; i < ctx.take_send_rst_s_ctrl_format().size(); i++) {
            if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_RST(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_SContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_SContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_S(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            } else if (ctx.take_send_rst_s_ctrl_format(i) instanceof SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) {
                SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext c = (SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext) ctx.take_send_rst_s_ctrl_format(i);
                ST el = visitTake_send_rst_s_ctrl_format_CONTROL(c);
                el.add("dation", getUserVariable(dation));
                st.add("elements", el);
            }
        }

        if ( ctx.ID() != null ) {
            ST el = group.getInstanceOf("take_id");
            el.add("id", getUserVariable(ctx.ID().getText()));
            el.add("dation", getUserVariable(dation));
            st.add("elements", el);
        }

        return st;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // RST-S-CTRL-Format ::=
    //     RST ( Name§ErrorVariable-FIXED )
    //   | S ( Name§Variable-FIXED )
    //   | CONTROL ( Expression [ , Expression [ , Expression ] ] )
    ////////////////////////////////////////////////////////////////////////////////

    @Override
    public ST visitTake_send_rst_s_ctrl_format_RST(SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext ctx) {
        ST st = group.getInstanceOf("take_send_rst_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public ST visitTake_send_rst_s_ctrl_format_S(SmallPearlParser.Take_send_rst_s_ctrl_format_SContext ctx) {
        ST st = group.getInstanceOf("take_send_s_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public ST visitTake_send_rst_s_ctrl_format_CONTROL(SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext ctx) {
        ST st = group.getInstanceOf("take_send_control_position");

        if ( ctx.expression().size() == 3  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
            st.add("expression3", ctx.expression(2).getText());
        }
        else if ( ctx.expression().size() == 2  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
        }
        else if ( ctx.expression().size() == 1  ) {
            st.add("expression1", ctx.expression(0).getText());
        }

        return st;
    }



    ////////////////////////////////////////////////////////////////////////////////
    // ReadStatement ::=
    //   READ [ { Name§Variable | Segment } [ , { Name§Variable | Segment } ] ... ] FROM Name§Dation [ BY Position [ , Position ] ... ] ;
    ////////////////////////////////////////////////////////////////////////////////

    @Override
    public ST visitReadStatement(SmallPearlParser.ReadStatementContext ctx) {
        ST st = group.getInstanceOf("read_statement");
        String dation = "";

        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Read_fromContext) {
                dation = ((SmallPearlParser.Read_fromContext)c).ID().getText();
            }
        }

        st.add("dation", getUserVariable(dation));

        for (int i = 0; i < ctx.readPosition().size(); i++) {
            if ( ctx.readPosition(i) instanceof SmallPearlParser.ReadAbsPositionContext) {
                SmallPearlParser.ReadAbsPositionContext c = (SmallPearlParser.ReadAbsPositionContext) ctx.readPosition(i);
                if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionCOLContext ) {
                    ST el = visitReadWriteAbsolutePositionCOL((SmallPearlParser.ReadWriteAbsolutePositionCOLContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionLINEContext ) {
                    ST el = visitReadWriteAbsolutePositionLINE((SmallPearlParser.ReadWriteAbsolutePositionLINEContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionPOSContext ) {
                    ST el = visitReadWriteAbsolutePositionPOS((SmallPearlParser.ReadWriteAbsolutePositionPOSContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionSOPContext ) {
                    ST el = visitReadWriteAbsolutePositionSOP((SmallPearlParser.ReadWriteAbsolutePositionSOPContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
            }
            else if ( ctx.readPosition(i) instanceof SmallPearlParser.ReadRelPositionContext) {
                SmallPearlParser.ReadRelPositionContext c = (SmallPearlParser.ReadRelPositionContext) ctx.readPosition(i);
                if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionXContext ) {
                    ST el = visitReadWriteRelativePositionX((SmallPearlParser.ReadWriteRelativePositionXContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionSKIPContext ) {
                    ST el = visitReadWriteRelativePositionSKIP((SmallPearlParser.ReadWriteRelativePositionSKIPContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionPAGEContext ) {
                    ST el = visitReadWriteRelativePositionPAGE((SmallPearlParser.ReadWriteRelativePositionPAGEContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionADVContext ) {
                    ST el = visitReadWriteRelativePositionADV((SmallPearlParser.ReadWriteRelativePositionADVContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(dation));
                    st.add("elements", el);
                }
            }
            else if ( ctx.readPosition(i) instanceof SmallPearlParser.ReadRSTPositionContext) {
                ST el = visitReadRSTPosition((SmallPearlParser.ReadRSTPositionContext) ctx.readPosition(i));
                el.add( "dation", getUserVariable(dation));
                st.add("elements", el);
            }
        }

        for (int i = 0; i < ctx.ID().size(); i++) {
            ST el = group.getInstanceOf("read_expression");
            el.add( "id", getUserVariable(ctx.ID(i).getText()));
            el.add( "dation", getUserVariable(dation));
            st.add( "elements", el);
        }

        return st;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // WriteStatement ::=
    //  WRITE [ { Expression | Segment } [ , { Expression | Segment } ] ... ]
    //     TO Name§Dation [ BY Position [ , Position ] ... ] ;
    //
    // Segment ::=
    //   Name§Field ( [ Index , ] ... Index : Index)
    //
    // Index ::=
    //   Expression§WithIntegerAsValue
    ////////////////////////////////////////////////////////////////////////////////

    @Override
    public ST visitWriteStatement(SmallPearlParser.WriteStatementContext ctx) {
        ST st = group.getInstanceOf("write_statement");
        st.add("dation", getUserVariable(ctx.ID().toString()));


        for (int i = 0; i < ctx.writePosition().size(); i++) {
            if ( ctx.writePosition(i) instanceof SmallPearlParser.WriteAbsPositionContext) {
                SmallPearlParser.WriteAbsPositionContext c = (SmallPearlParser.WriteAbsPositionContext) ctx.writePosition(i);
                if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionCOLContext ) {
                    ST el = visitReadWriteAbsolutePositionCOL((SmallPearlParser.ReadWriteAbsolutePositionCOLContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionLINEContext ) {
                    ST el = visitReadWriteAbsolutePositionLINE((SmallPearlParser.ReadWriteAbsolutePositionLINEContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionPOSContext ) {
                    ST el = visitReadWriteAbsolutePositionPOS((SmallPearlParser.ReadWriteAbsolutePositionPOSContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                } else if ( c.readWriteAbsolutePosition() instanceof  SmallPearlParser.ReadWriteAbsolutePositionSOPContext ) {
                    ST el = visitReadWriteAbsolutePositionSOP((SmallPearlParser.ReadWriteAbsolutePositionSOPContext) c.readWriteAbsolutePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
            }
            else if ( ctx.writePosition(i) instanceof SmallPearlParser.WriteRelPositionContext) {
                SmallPearlParser.WriteRelPositionContext c = (SmallPearlParser.WriteRelPositionContext) ctx.writePosition(i);
                if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionXContext ) {
                    ST el = visitReadWriteRelativePositionX((SmallPearlParser.ReadWriteRelativePositionXContext) c.readWriteRelativePosition());
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionSKIPContext ) {
                    ST el = visitReadWriteRelativePositionSKIP((SmallPearlParser.ReadWriteRelativePositionSKIPContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionPAGEContext ) {
                    ST el = visitReadWriteRelativePositionPAGE((SmallPearlParser.ReadWriteRelativePositionPAGEContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
                else if ( c.readWriteRelativePosition() instanceof  SmallPearlParser.ReadWriteRelativePositionADVContext ) {
                    ST el = visitReadWriteRelativePositionADV((SmallPearlParser.ReadWriteRelativePositionADVContext) c.readWriteRelativePosition());
                    el.add( "dation", getUserVariable(ctx.ID().toString()));
                    st.add("elements", el);
                }
            }
            else if ( ctx.writePosition(i) instanceof SmallPearlParser.WriteRSTPositionContext) {
                ST el = visitWriteRSTPosition((SmallPearlParser.WriteRSTPositionContext) ctx.writePosition(i));
                el.add( "dation", getUserVariable(ctx.ID().toString()));
                st.add("elements", el);
            }
        }

        for (int i = 0; i < ctx.expression().size(); i++) {
            ST el = group.getInstanceOf("write_expression");
            el.add( "expression", getUserVariable(ctx.expression(i).getText()));
            el.add( "dation", getUserVariable(ctx.ID().toString()));
            st.add( "elements", el);
        }

        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionCOL( SmallPearlParser.ReadWriteAbsolutePositionCOLContext ctx) {
        ST st = group.getInstanceOf("read_write_col_position");
        st.add("expression", ctx.expression().getText());
        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionLINE( SmallPearlParser.ReadWriteAbsolutePositionLINEContext ctx) {
        ST st = group.getInstanceOf("read_write_line_position");
        st.add("expression", ctx.expression().getText());
        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionPOS( SmallPearlParser.ReadWriteAbsolutePositionPOSContext ctx) {
        ST st = group.getInstanceOf("read_write_pos_position");

        if ( ctx.expression().size() == 3  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
            st.add("expression3", ctx.expression(2).getText());
        }
        else if ( ctx.expression().size() == 2  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
        }
        else if ( ctx.expression().size() == 1  ) {
            st.add("expression1", ctx.expression(0).getText());
        }

        return st;
    }

    @Override
    public
    ST visitReadWriteAbsolutePositionSOP( SmallPearlParser.ReadWriteAbsolutePositionSOPContext ctx) {
        ST st = group.getInstanceOf("read_write_sop_position");

        if ( ctx.ID().size() == 3  ) {
            st.add("id1", getUserVariable(ctx.ID(0).getText()));
            st.add("id2", getUserVariable(ctx.ID(1).getText()));
            st.add("id3", getUserVariable(ctx.ID(2).getText()));
        }
        else if ( ctx.ID().size() == 2  ) {
            st.add("id1", getUserVariable(ctx.ID(0).getText()));
            st.add("id2", getUserVariable(ctx.ID(1).getText()));
        }
        else if ( ctx.ID().size() == 1  ) {
            st.add("id1", getUserVariable(ctx.ID(0).getText()));
        }

        return st;
    }

    @Override
    public
    ST visitReadRSTPosition( SmallPearlParser.ReadRSTPositionContext ctx) {
        ST st = group.getInstanceOf("read_write_rst_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public
    ST visitWriteRSTPosition( SmallPearlParser.WriteRSTPositionContext ctx) {
        ST st = group.getInstanceOf("read_write_rst_position");
        st.add( "id", getUserVariable(ctx.ID().getText()));
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionX(SmallPearlParser.ReadWriteRelativePositionXContext ctx) {
        ST st = group.getInstanceOf("read_write_x_position");
        if ( ctx.expression() != null ) {
            st.add("expression", ctx.expression().getText());
        }
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionSKIP(SmallPearlParser.ReadWriteRelativePositionSKIPContext ctx) {
        ST st = group.getInstanceOf("read_write_skip_position");
        if ( ctx.expression() != null ) {
            st.add("expression", ctx.expression().getText());
        }
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionPAGE(SmallPearlParser.ReadWriteRelativePositionPAGEContext ctx) {
        ST st = group.getInstanceOf("read_write_page_position");
        if ( ctx.expression() != null ) {
            st.add("expression", ctx.expression().getText());
        }
        return st;
    }

    @Override
    public
    ST visitReadWriteRelativePositionADV(SmallPearlParser.ReadWriteRelativePositionADVContext ctx) {
        ST st = group.getInstanceOf("read_write_adv_position");

        if ( ctx.expression().size() == 3  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
            st.add("expression3", ctx.expression(2).getText());
        }
        else if ( ctx.expression().size() == 2  ) {
            st.add("expression1", ctx.expression(0).getText());
            st.add("expression2", ctx.expression(1).getText());
        }
        else if ( ctx.expression().size() == 1  ) {
            st.add("expression1", ctx.expression(0).getText());
        }

        return st;
    }

    @Override
    public ST visitCallStatement(SmallPearlParser.CallStatementContext ctx) {
        ST stmt = group.getInstanceOf("CallStatement");
// TODO: visitCallStatement: Implement
//        stmt.add("callee", ctx.ID());
//        stmt.add("args", visitList_of_actual_parameters(ctx.list_of_actual_parameters()));

        throw new NotYetImplementedException("callStatement", ctx.start.getLine(), ctx.start.getCharPositionInLine());

        // return stmt;
    }

    @Override
    public ST visitCpp_inline(SmallPearlParser.Cpp_inlineContext ctx) {
        ST stmt = group.getInstanceOf("cpp_inline");

        stmt.add("body", "#warning __cpp__ inline inserted");

        int i;
        for (i = 0; i < ctx.StringLiteral().size(); i++) {
            String line = ctx.StringLiteral(i).toString();

            line = line.replaceAll("^'", "");
            line = line.replaceAll("'$", "");

            line = unescapeString(line);

            stmt.add("body", line);
        }

        return stmt;
    }

    @Override
    public ST visitAtanExpression(SmallPearlParser.AtanExpressionContext ctx) {
        ST st = group.getInstanceOf("ATAN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitCosExpression(SmallPearlParser.CosExpressionContext ctx) {
        ST st = group.getInstanceOf("COS");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitExpExpression(SmallPearlParser.ExpExpressionContext ctx) {
        ST st = group.getInstanceOf("EXP");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitLnExpression(SmallPearlParser.LnExpressionContext ctx) {
        ST st = group.getInstanceOf("LN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitSinExpression(SmallPearlParser.SinExpressionContext ctx) {
        ST st = group.getInstanceOf("SIN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitSqrtExpression(SmallPearlParser.SqrtExpressionContext ctx) {
        ST st = group.getInstanceOf("SQRT");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitTanExpression(SmallPearlParser.TanExpressionContext ctx) {
        ST st = group.getInstanceOf("TAN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitTanhExpression(SmallPearlParser.TanhExpressionContext ctx) {
        ST st = group.getInstanceOf("TANH");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitAbsExpression(SmallPearlParser.AbsExpressionContext ctx) {
        ST st = group.getInstanceOf("ABS");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitSignExpression(SmallPearlParser.SignExpressionContext ctx) {
        ST st = group.getInstanceOf("SIGN");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitRemainderExpression(SmallPearlParser.RemainderExpressionContext ctx) {
        ST st = group.getInstanceOf("REM");
        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));
        return st;
    }

    @Override
    public ST visitSizeofExpression(SmallPearlParser.SizeofExpressionContext ctx) {
        ST st = group.getInstanceOf("SIZEOF");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitEntierExpression(SmallPearlParser.EntierExpressionContext ctx) {
        ST st = group.getInstanceOf("ENTIER");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitRoundExpression(SmallPearlParser.RoundExpressionContext ctx) {
        ST st = group.getInstanceOf("ROUND");
        st.add("operand", visit(ctx.getChild(1)));
        return st;
    }

    @Override
    public ST visitEqRelationalExpression(SmallPearlParser.EqRelationalExpressionContext ctx) {
        ST st = group.getInstanceOf("EQ");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitNeRelationalExpression(SmallPearlParser.NeRelationalExpressionContext ctx) {
        ST st = group.getInstanceOf("NE");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitLtRelationalExpression(SmallPearlParser.LtRelationalExpressionContext ctx) {
        ST st = group.getInstanceOf("LT");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitLeRelationalExpression(SmallPearlParser.LeRelationalExpressionContext ctx) {
        ST st = group.getInstanceOf("LE");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitGtRelationalExpression(SmallPearlParser.GtRelationalExpressionContext ctx) {
        ST st = group.getInstanceOf("GT");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitGeRelationalExpression(SmallPearlParser.GeRelationalExpressionContext ctx) {
        ST st = group.getInstanceOf("GE");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitFitExpression(SmallPearlParser.FitExpressionContext ctx) {
        ST st = group.getInstanceOf("FIT");

        st.add("lhs", visit(ctx.expression(0)));
        st.add("rhs", visit(ctx.expression(1)));

        return st;
    }

    @Override
    public ST visitNowFunction(SmallPearlParser.NowFunctionContext ctx) {
        ST st = group.getInstanceOf("NOW");
        return st;
    }

//    dationSpecification
//    : ( 'SPECIFY' | 'SPC' ) identifierDenotation typeDation globalAttribute? ';'
//    ;

    @Override
    public ST visitDationSpecification(SmallPearlParser.DationSpecificationContext ctx) {
        ST dationSpecification = group.getInstanceOf("DationSpecifications");
        ST typeDation = group.getInstanceOf("TypeDation");
        boolean hasGlobalAttribute = false;

        ArrayList<String> identifierDenotationList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {

                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                } else if (c instanceof SmallPearlParser.TypeDationContext) {
                    typeDation = visitTypeDation((SmallPearlParser.TypeDationContext) c);
                } else if (c instanceof SmallPearlParser.GlobalAttributeContext) {
                    hasGlobalAttribute = true;
                }
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                ST v;
                ST t = (ST) typeDation.getAttribute("ClassAttribute");
                if ( t.getAttribute("basic") == "1" ) {
                    v = group.getInstanceOf("DationSpecificationBasic");
                    v.add("name", identifierDenotationList.get(i));
                }
                else {
                    v = group.getInstanceOf("DationSpecificationNonBasic");
                    v.add("name", identifierDenotationList.get(i));
                }

                v.add("TypeDation", typeDation);
                v.add("global", hasGlobalAttribute);

                dationSpecification.add("decl", v);
            }
        }

        return dationSpecification;
    }


////////////////////////////////////////////////////////////////////////////////
// TODO:  TypeDation ::=
//    DATION SourceSinkAttribute ClassAttribute [ Structure ] [ AccessAttribute ]
////////////////////////////////////////////////////////////////////////////////

//    typeDation
//    : 'DATION' sourceSinkAttribute classAttribute  accessAttribute?
//    ;

////////////////////////////////////////////////////////////////////////////////
// TODO:  SourceSinkAttribute ::=
//    IN | OUT | INOUT
////////////////////////////////////////////////////////////////////////////////

//    sourceSinkAttribute
//    : 'IN' | 'OUT' | 'INOUT'
//            : 'IN' | 'OUT' | 'INOUT'
//    ;

////////////////////////////////////////////////////////////////////////////////
//  ClassAttribute ::=
//    ALPHIC | BASIC | TypeOfTransmissionData
//    ALPHIC | BASIC | TypeOfTransmissionData
////////////////////////////////////////////////////////////////////////////////

//    classAttribute
//    : 'ALPHIC' | 'BASIC' | typeOfTransmissionData
//    ;

////////////////////////////////////////////////////////////////////////////////
//  TypeOfTransmissionData ::=
//    ALL | SimpleType | CompoundType
////////////////////////////////////////////////////////////////////////////////

//    typeOfTransmissionData
//    : 'ALL' | simpleType | compoundType
//    ;

////////////////////////////////////////////////////////////////////////////////
// AccessAttribute ::=
//  { DIRECT | FORWARD | FORBACK } [ NOCYCL | CYCLIC ] [ STREAM | NOSTREAM ]
////////////////////////////////////////////////////////////////////////////////

//    accessAttribute
//    : ( 'DIRECT' | 'FORWARD' | 'FORBACK' ) ( 'NOCYCL' | 'CYCLIC' )? ( 'STREAM' | 'NOSTREAM' )?
//    ;

////////////////////////////////////////////////////////////////////////////////
// TODO:  DationDeclaration ::=
//    {DECLARE | DCL} IdentifierDenotation TypeDation [GlobalAttribute] CREATED (Name§SystemDefDation);
////////////////////////////////////////////////////////////////////////////////

//    dationDeclaration
//    : ( 'DECLARE' | 'DCL' ) identifierDenotation typeDation globalAttribute? 'CREATED' '(' ID  ')' ';'
//    ;

    @Override
    public ST visitDationDeclaration(SmallPearlParser.DationDeclarationContext ctx) {
        ST dationDeclarations = group.getInstanceOf("DationDeclarations");
        ST typeDation = group.getInstanceOf("TypeDation");
        ST typology = group.getInstanceOf("Typology");
        ST accessAttributes = group.getInstanceOf("AccessAttributes");

        dationDeclarations.add("decl", visitIdentifierDenotation(ctx.identifierDenotation()));
        typeDation = visitTypeDation(ctx.typeDation());
        typology = visitTypology(ctx.typology());

        if (ctx.accessAttribute() != null) {
            accessAttributes = visitAccessAttribute(ctx.accessAttribute());
        }

        if (ctx.globalAttribute() != null) {
            visitGlobalAttribute(ctx.globalAttribute());
        }

        ArrayList<String> identifierDenotationList = null;

        if (ctx != null) {
            for (ParseTree c : ctx.children) {
                if (c instanceof SmallPearlParser.IdentifierDenotationContext) {
                    identifierDenotationList = getIdentifierDenotation((SmallPearlParser.IdentifierDenotationContext) c);
                }
            }

            for (int i = 0; i < identifierDenotationList.size(); i++) {
                ST v = group.getInstanceOf("DationDeclaration");
                v.add("name", identifierDenotationList.get(i));
                v.add("TypeDation", typeDation);
                typology.add("name", identifierDenotationList.get(i));
                v.add("Typology", typology);
                v.add("Id", ctx.ID().getText());
                v.add("Dation", getDationClass(ctx.typeDation().classAttribute()));

                typeDation.add("AccessAttribute", accessAttributes);
                typeDation.add("Dim", identifierDenotationList.get(i));

                dationDeclarations.add("decl", v);
            }
        }

        return dationDeclarations;
    }

    @Override
    public ST visitTypeDation(SmallPearlParser.TypeDationContext ctx) {
        ST st = group.getInstanceOf("TypeDation");

        st.add("SourceSinkAttribute", ctx.sourceSinkAttribute().getText());

        if ( ctx.systemDation() != null ) {
            st.add( "SystemDation", "");
        }

        st.add("ClassAttribute", getClassAttribute(ctx.classAttribute()));

        return st;
    }

    @Override
    public ST visitTypology(SmallPearlParser.TypologyContext ctx) {
        ST st = group.getInstanceOf("Typology");
        for (ParseTree c : ctx.children) {
            if (c instanceof SmallPearlParser.Dimension1StarContext) {
                st.add("DIM1", -1);
                st.add("DIM1Unlimited", 1);
            } else if (c instanceof SmallPearlParser.Dimension1IntegerContext) {
                st.add("DIM1", Integer.valueOf(((SmallPearlParser.Dimension1IntegerContext) c).IntegerConstant().toString()));
            } else if (c instanceof SmallPearlParser.Dimension2IntegerContext) {
                st.add("DIM2", Integer.valueOf(((SmallPearlParser.Dimension2IntegerContext) c).IntegerConstant().toString()));
            } else if (c instanceof SmallPearlParser.Dimension3IntegerContext) {
                st.add("DIM3", Integer.valueOf(((SmallPearlParser.Dimension3IntegerContext) c).IntegerConstant().toString()));
            }
        }

        return st;
    }

    private String getDationClass(SmallPearlParser.ClassAttributeContext ctx) {
        if (ctx instanceof SmallPearlParser.ClassAttributeALPHICContext) {
            return "DationPG";
        } else {
            return "DationRW";
        }
    }

    private ST getStepSize(SmallPearlParser.ClassAttributeContext ctx) {
        ST st = group.getInstanceOf("StepSize");

        st.add("type", "Fixed");
        st.add("size", "31");

        return st;
    }

    private ST getClassAttribute(SmallPearlParser.ClassAttributeContext ctx) {
        ST st = group.getInstanceOf("ClassAttribute");

        if (ctx instanceof SmallPearlParser.ClassAttributeALPHICContext) {
            st.add("alphic", "1");
        } else if (ctx instanceof SmallPearlParser.ClassAttributeBASICContext) {
            st.add("basic", "1");
        } else if (ctx instanceof SmallPearlParser.ClassAttributeTypeOfTransmissionDataContext) {


            st.add("attribute", getClassAttributeTypeOfTransmissionData((SmallPearlParser.ClassAttributeTypeOfTransmissionDataContext) ctx));
        }

        return st;
    }


    private ST getClassAttributeTypeOfTransmissionData(SmallPearlParser.ClassAttributeTypeOfTransmissionDataContext ctx) {
        ST st = group.getInstanceOf("TypeOfTransmissionData");

        if (ctx.typeOfTransmissionData() instanceof SmallPearlParser.TypeOfTransmissionDataALLContext) {
            st.add("all", "1");
            st.add("size", "0");
        } else if (ctx.typeOfTransmissionData() instanceof SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext) {
            SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext c = (SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext) ctx.typeOfTransmissionData();
            st.add("size", "0");
            st.add("type", visitSimpleType(c.simpleType()));
        } else if (ctx.typeOfTransmissionData() instanceof SmallPearlParser.TypeOfTransmissionDataCompoundTypeContext) {
            throw new NotYetImplementedException("TypeOfTransmissionData:CompoundType", ctx.start.getLine(), ctx.start.getCharPositionInLine());
        }

        return st;
    }

    private ST getAccessAttribute(SmallPearlParser.AccessAttributeContext ctx) {
        ST st = group.getInstanceOf("AccessAttribute");

        for (ParseTree c : ctx.children) {
            st.add("attribute", c.getText());
        }

        return st;
    }

    @Override
    public ST visitAccessAttribute(SmallPearlParser.AccessAttributeContext ctx) {
        ST st = group.getInstanceOf("AccessAttribute");

        for (ParseTree c : ctx.children) {
            st.add("attribute", c.getText());
        }

        return st;
    }

    @Override
    public ST visitBlock_statement(SmallPearlParser.Block_statementContext ctx) {
        ST st = group.getInstanceOf("block_statement");

        for (ParseTree c : ctx.children) {
            if ( c instanceof SmallPearlParser.ScalarVariableDeclarationContext ) {
                st.add("code", visitScalarVariableDeclaration((SmallPearlParser.ScalarVariableDeclarationContext)c));
            }
            else if ( c instanceof SmallPearlParser.StatementContext ) {
                st.add("code", visitStatement((SmallPearlParser.StatementContext)c));
            }
        }

        return st;
    }

    private class Format {
        private String format_;
        private ST format1_;

    }

    private String unescapeString(String st) {
        StringBuilder sb = new StringBuilder(st.length());

        for (int i = 0; i < st.length(); i++) {
            char ch = st.charAt(i);
            if (ch == '\\') {
                char nextChar = (i == st.length() - 1) ? '\\' : st.charAt(i + 1);

                switch (nextChar) {
                    case '\\':
                        ch = '\\';
                        break;
                    case 'b':
                        ch = '\b';
                        break;
                    case 'f':
                        ch = '\f';
                        break;
                    case 'r':
                        ch = '\r';
                        break;
                    case 't':
                        ch = '\t';
                        break;
                    case '\"':
                        ch = '\"';
                        break;
                    case '\'':
                        ch = '\'';
                        break;
                }

                i++;
            }
            sb.append(ch);
        }
        return sb.toString();
    }
}
