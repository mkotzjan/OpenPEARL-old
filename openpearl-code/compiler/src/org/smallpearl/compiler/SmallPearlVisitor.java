// Generated from SmallPearl.g4 by ANTLR 4.5

    package org.smallpearl.compiler;
    import org.smallpearl.compiler.OpenPearlLexer;

import org.antlr.v4.runtime.misc.NotNull;
import org.antlr.v4.runtime.tree.ParseTreeVisitor;

/**
 * This interface defines a complete generic visitor for a parse tree produced
 * by {@link SmallPearlParser}.
 *
 * @param <T> The return type of the visit operation. Use {@link Void} for
 * operations with no return type.
 */
public interface SmallPearlVisitor<T> extends ParseTreeVisitor<T> {
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#program}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitProgram(SmallPearlParser.ProgramContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#module}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitModule(SmallPearlParser.ModuleContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#system_part}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSystem_part(SmallPearlParser.System_partContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#problem_part}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitProblem_part(SmallPearlParser.Problem_partContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#username_declaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUsername_declaration(SmallPearlParser.Username_declarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#username_declaration_without_data_flow_direction}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUsername_declaration_without_data_flow_direction(SmallPearlParser.Username_declaration_without_data_flow_directionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#username_declaration_with_data_flow_direction}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUsername_declaration_with_data_flow_direction(SmallPearlParser.Username_declaration_with_data_flow_directionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code userConfigurationWithoutAssociation}
	 * labeled alternative in {@link SmallPearlParser#user_configuration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUserConfigurationWithoutAssociation(SmallPearlParser.UserConfigurationWithoutAssociationContext ctx);
	/**
	 * Visit a parse tree produced by the {@code userConfigurationWithAssociation}
	 * labeled alternative in {@link SmallPearlParser#user_configuration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUserConfigurationWithAssociation(SmallPearlParser.UserConfigurationWithAssociationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#user_configuration_without_association}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUser_configuration_without_association(SmallPearlParser.User_configuration_without_associationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#user_configuration_with_association}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUser_configuration_with_association(SmallPearlParser.User_configuration_with_associationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#username_parameters}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUsername_parameters(SmallPearlParser.Username_parametersContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#identification}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIdentification(SmallPearlParser.IdentificationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#allocation_protection}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAllocation_protection(SmallPearlParser.Allocation_protectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#identification_attribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIdentification_attribute(SmallPearlParser.Identification_attributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#scalarVariableDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitScalarVariableDeclaration(SmallPearlParser.ScalarVariableDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#variableDenotation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitVariableDenotation(SmallPearlParser.VariableDenotationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#allocationProtection}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAllocationProtection(SmallPearlParser.AllocationProtectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#globalAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGlobalAttribute(SmallPearlParser.GlobalAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeAttribute(SmallPearlParser.TypeAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#simpleType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSimpleType(SmallPearlParser.SimpleTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeInteger}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeInteger(SmallPearlParser.TypeIntegerContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#mprecision}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMprecision(SmallPearlParser.MprecisionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#integerWithoutPrecision}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIntegerWithoutPrecision(SmallPearlParser.IntegerWithoutPrecisionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeFloatingPointNumber}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeFloatingPointNumber(SmallPearlParser.TypeFloatingPointNumberContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeBitString}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeBitString(SmallPearlParser.TypeBitStringContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeCharacterString}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeCharacterString(SmallPearlParser.TypeCharacterStringContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeDuration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeDuration(SmallPearlParser.TypeDurationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#identifierDenotation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIdentifierDenotation(SmallPearlParser.IdentifierDenotationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#initialisationAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitInitialisationAttribute(SmallPearlParser.InitialisationAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#arrayInitialisationAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArrayInitialisationAttribute(SmallPearlParser.ArrayInitialisationAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#initElement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitInitElement(SmallPearlParser.InitElementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#variable_init}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitVariable_init(SmallPearlParser.Variable_initContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#structVariableDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStructVariableDeclaration(SmallPearlParser.StructVariableDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#structureDenotation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStructureDenotation(SmallPearlParser.StructureDenotationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeStructure}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeStructure(SmallPearlParser.TypeStructureContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#structureComponent}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStructureComponent(SmallPearlParser.StructureComponentContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeAttributeInStructureComponent}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeAttributeInStructureComponent(SmallPearlParser.TypeAttributeInStructureComponentContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#structuredType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStructuredType(SmallPearlParser.StructuredTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#structureSpecfication}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStructureSpecfication(SmallPearlParser.StructureSpecficationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#structureDenotationS}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStructureDenotationS(SmallPearlParser.StructureDenotationSContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#arrayVariableDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArrayVariableDeclaration(SmallPearlParser.ArrayVariableDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#arrayDenotation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArrayDenotation(SmallPearlParser.ArrayDenotationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeAttributeForArray}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeAttributeForArray(SmallPearlParser.TypeAttributeForArrayContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReference}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReference(SmallPearlParser.TypeReferenceContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferences}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferences(SmallPearlParser.TypeReferencesContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceSimpleType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceSimpleType(SmallPearlParser.TypeReferenceSimpleTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceStructuredType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceStructuredType(SmallPearlParser.TypeReferenceStructuredTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceDationType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceDationType(SmallPearlParser.TypeReferenceDationTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceSemaType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceSemaType(SmallPearlParser.TypeReferenceSemaTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceBoltType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceBoltType(SmallPearlParser.TypeReferenceBoltTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceProcedureType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceProcedureType(SmallPearlParser.TypeReferenceProcedureTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceTaskType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceTaskType(SmallPearlParser.TypeReferenceTaskTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceInterruptType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceInterruptType(SmallPearlParser.TypeReferenceInterruptTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceSignalType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceSignalType(SmallPearlParser.TypeReferenceSignalTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeReferenceCharType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeReferenceCharType(SmallPearlParser.TypeReferenceCharTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#semaDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSemaDeclaration(SmallPearlParser.SemaDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#preset}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPreset(SmallPearlParser.PresetContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#procedureDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitProcedureDeclaration(SmallPearlParser.ProcedureDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#procedureBody}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitProcedureBody(SmallPearlParser.ProcedureBodyContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#listOfFormalParameters}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitListOfFormalParameters(SmallPearlParser.ListOfFormalParametersContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#formalParameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormalParameter(SmallPearlParser.FormalParameterContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#assignmentProtection}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssignmentProtection(SmallPearlParser.AssignmentProtectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#passIdentical}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPassIdentical(SmallPearlParser.PassIdenticalContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#parameterType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitParameterType(SmallPearlParser.ParameterTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#disableStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDisableStatement(SmallPearlParser.DisableStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#enableStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEnableStatement(SmallPearlParser.EnableStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#triggerStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTriggerStatement(SmallPearlParser.TriggerStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#resultAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitResultAttribute(SmallPearlParser.ResultAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#resultType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitResultType(SmallPearlParser.ResultTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#taskDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTaskDeclaration(SmallPearlParser.TaskDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#task_main}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTask_main(SmallPearlParser.Task_mainContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#taskBody}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTaskBody(SmallPearlParser.TaskBodyContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStatement(SmallPearlParser.StatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#unlabeled_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnlabeled_statement(SmallPearlParser.Unlabeled_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#empty_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEmpty_statement(SmallPearlParser.Empty_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#label_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLabel_statement(SmallPearlParser.Label_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#callStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCallStatement(SmallPearlParser.CallStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#listOfActualParameters}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitListOfActualParameters(SmallPearlParser.ListOfActualParametersContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#returnStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReturnStatement(SmallPearlParser.ReturnStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#gotoStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGotoStatement(SmallPearlParser.GotoStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#exitStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExitStatement(SmallPearlParser.ExitStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#assignment_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssignment_statement(SmallPearlParser.Assignment_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#dereference}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDereference(SmallPearlParser.DereferenceContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#stringSelection}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStringSelection(SmallPearlParser.StringSelectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#bitSelection}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBitSelection(SmallPearlParser.BitSelectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#charSelection}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCharSelection(SmallPearlParser.CharSelectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#sequential_control_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSequential_control_statement(SmallPearlParser.Sequential_control_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#if_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIf_statement(SmallPearlParser.If_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#then_block}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitThen_block(SmallPearlParser.Then_blockContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#else_block}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitElse_block(SmallPearlParser.Else_blockContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_statement(SmallPearlParser.Case_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_statement_selection1}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_statement_selection1(SmallPearlParser.Case_statement_selection1Context ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_statement_selection1_alt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_statement_selection1_alt(SmallPearlParser.Case_statement_selection1_altContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_statement_selection_out}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_statement_selection_out(SmallPearlParser.Case_statement_selection_outContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_statement_selection2}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_statement_selection2(SmallPearlParser.Case_statement_selection2Context ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_statement_selection2_alt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_statement_selection2_alt(SmallPearlParser.Case_statement_selection2_altContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#case_list}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase_list(SmallPearlParser.Case_listContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#index_section}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIndex_section(SmallPearlParser.Index_sectionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#block_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBlock_statement(SmallPearlParser.Block_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement(SmallPearlParser.LoopStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement_for}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement_for(SmallPearlParser.LoopStatement_forContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement_from}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement_from(SmallPearlParser.LoopStatement_fromContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement_by}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement_by(SmallPearlParser.LoopStatement_byContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement_to}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement_to(SmallPearlParser.LoopStatement_toContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement_while}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement_while(SmallPearlParser.LoopStatement_whileContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#loopStatement_end}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLoopStatement_end(SmallPearlParser.LoopStatement_endContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#realtime_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRealtime_statement(SmallPearlParser.Realtime_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#task_control_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTask_control_statement(SmallPearlParser.Task_control_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#task_terminating}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTask_terminating(SmallPearlParser.Task_terminatingContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#task_suspending}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTask_suspending(SmallPearlParser.Task_suspendingContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#taskContinuation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTaskContinuation(SmallPearlParser.TaskContinuationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#taskResume}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTaskResume(SmallPearlParser.TaskResumeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#task_preventing}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTask_preventing(SmallPearlParser.Task_preventingContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#taskStart}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTaskStart(SmallPearlParser.TaskStartContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#priority}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPriority(SmallPearlParser.PriorityContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#frequency}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFrequency(SmallPearlParser.FrequencyContext ctx);
	/**
	 * Visit a parse tree produced by the {@code startConditionAFTER}
	 * labeled alternative in {@link SmallPearlParser#startCondition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStartConditionAFTER(SmallPearlParser.StartConditionAFTERContext ctx);
	/**
	 * Visit a parse tree produced by the {@code startConditionAT}
	 * labeled alternative in {@link SmallPearlParser#startCondition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStartConditionAT(SmallPearlParser.StartConditionATContext ctx);
	/**
	 * Visit a parse tree produced by the {@code startConditionWHEN}
	 * labeled alternative in {@link SmallPearlParser#startCondition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStartConditionWHEN(SmallPearlParser.StartConditionWHENContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#task_coordination_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTask_coordination_statement(SmallPearlParser.Task_coordination_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#semaRequest}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSemaRequest(SmallPearlParser.SemaRequestContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#semaRelease}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSemaRelease(SmallPearlParser.SemaReleaseContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#semaTry}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSemaTry(SmallPearlParser.SemaTryContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#boltDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBoltDeclaration(SmallPearlParser.BoltDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#boltReserve}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBoltReserve(SmallPearlParser.BoltReserveContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#boltFree}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBoltFree(SmallPearlParser.BoltFreeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#boltEnter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBoltEnter(SmallPearlParser.BoltEnterContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#boltLeave}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBoltLeave(SmallPearlParser.BoltLeaveContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#interrupt_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitInterrupt_statement(SmallPearlParser.Interrupt_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#io_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIo_statement(SmallPearlParser.Io_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#open_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_statement(SmallPearlParser.Open_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#open_parameterlist}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameterlist(SmallPearlParser.Open_parameterlistContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_idf}
	 * labeled alternative in {@link SmallPearlParser#open_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_idf(SmallPearlParser.Open_parameter_idfContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_rst}
	 * labeled alternative in {@link SmallPearlParser#open_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_rst(SmallPearlParser.Open_parameter_rstContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_old_or_new_or_any}
	 * labeled alternative in {@link SmallPearlParser#open_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_old_or_new_or_any(SmallPearlParser.Open_parameter_old_or_new_or_anyContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_can_or_prm}
	 * labeled alternative in {@link SmallPearlParser#open_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_can_or_prm(SmallPearlParser.Open_parameter_can_or_prmContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_old}
	 * labeled alternative in {@link SmallPearlParser#open_parameter_old_new_any}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_old(SmallPearlParser.Open_parameter_oldContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_new}
	 * labeled alternative in {@link SmallPearlParser#open_parameter_old_new_any}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_new(SmallPearlParser.Open_parameter_newContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_any}
	 * labeled alternative in {@link SmallPearlParser#open_parameter_old_new_any}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_any(SmallPearlParser.Open_parameter_anyContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_can}
	 * labeled alternative in {@link SmallPearlParser#open_parameter_can_prm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_can(SmallPearlParser.Open_parameter_canContext ctx);
	/**
	 * Visit a parse tree produced by the {@code open_parameter_prm}
	 * labeled alternative in {@link SmallPearlParser#open_parameter_can_prm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOpen_parameter_prm(SmallPearlParser.Open_parameter_prmContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#close_statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClose_statement(SmallPearlParser.Close_statementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#close_parameterlist}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClose_parameterlist(SmallPearlParser.Close_parameterlistContext ctx);
	/**
	 * Visit a parse tree produced by the {@code close_parameter_can}
	 * labeled alternative in {@link SmallPearlParser#close_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClose_parameter_can(SmallPearlParser.Close_parameter_canContext ctx);
	/**
	 * Visit a parse tree produced by the {@code close_parameter_prm}
	 * labeled alternative in {@link SmallPearlParser#close_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClose_parameter_prm(SmallPearlParser.Close_parameter_prmContext ctx);
	/**
	 * Visit a parse tree produced by the {@code close_parameter_rst}
	 * labeled alternative in {@link SmallPearlParser#close_parameter}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClose_parameter_rst(SmallPearlParser.Close_parameter_rstContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#getStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGetStatement(SmallPearlParser.GetStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#getFrom}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGetFrom(SmallPearlParser.GetFromContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#putStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPutStatement(SmallPearlParser.PutStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code factorFormat}
	 * labeled alternative in {@link SmallPearlParser#formatPosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFactorFormat(SmallPearlParser.FactorFormatContext ctx);
	/**
	 * Visit a parse tree produced by the {@code factorPosition}
	 * labeled alternative in {@link SmallPearlParser#formatPosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFactorPosition(SmallPearlParser.FactorPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code factorFormatPosition}
	 * labeled alternative in {@link SmallPearlParser#formatPosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFactorFormatPosition(SmallPearlParser.FactorFormatPositionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#factor}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFactor(SmallPearlParser.FactorContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#format}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormat(SmallPearlParser.FormatContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionRST}
	 * labeled alternative in {@link SmallPearlParser#position}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionRST(SmallPearlParser.PositionRSTContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionSKIP}
	 * labeled alternative in {@link SmallPearlParser#position}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionSKIP(SmallPearlParser.PositionSKIPContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionX}
	 * labeled alternative in {@link SmallPearlParser#position}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionX(SmallPearlParser.PositionXContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionEOF}
	 * labeled alternative in {@link SmallPearlParser#position}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionEOF(SmallPearlParser.PositionEOFContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#fixedFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFixedFormat(SmallPearlParser.FixedFormatContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#fieldWidth}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFieldWidth(SmallPearlParser.FieldWidthContext ctx);
	/**
	 * Visit a parse tree produced by the {@code floatFormatE}
	 * labeled alternative in {@link SmallPearlParser#floatFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFloatFormatE(SmallPearlParser.FloatFormatEContext ctx);
	/**
	 * Visit a parse tree produced by the {@code floatFormatE3}
	 * labeled alternative in {@link SmallPearlParser#floatFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFloatFormatE3(SmallPearlParser.FloatFormatE3Context ctx);
	/**
	 * Visit a parse tree produced by the {@code bitFormat1}
	 * labeled alternative in {@link SmallPearlParser#bitFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBitFormat1(SmallPearlParser.BitFormat1Context ctx);
	/**
	 * Visit a parse tree produced by the {@code bitFormat2}
	 * labeled alternative in {@link SmallPearlParser#bitFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBitFormat2(SmallPearlParser.BitFormat2Context ctx);
	/**
	 * Visit a parse tree produced by the {@code bitFormat3}
	 * labeled alternative in {@link SmallPearlParser#bitFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBitFormat3(SmallPearlParser.BitFormat3Context ctx);
	/**
	 * Visit a parse tree produced by the {@code bitFormat4}
	 * labeled alternative in {@link SmallPearlParser#bitFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBitFormat4(SmallPearlParser.BitFormat4Context ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#numberOfCharacters}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNumberOfCharacters(SmallPearlParser.NumberOfCharactersContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#timeFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTimeFormat(SmallPearlParser.TimeFormatContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#durationFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDurationFormat(SmallPearlParser.DurationFormatContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#decimalPositions}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDecimalPositions(SmallPearlParser.DecimalPositionsContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#scaleFactor}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitScaleFactor(SmallPearlParser.ScaleFactorContext ctx);
	/**
	 * Visit a parse tree produced by the {@code characterStringFormatA}
	 * labeled alternative in {@link SmallPearlParser#characterStringFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCharacterStringFormatA(SmallPearlParser.CharacterStringFormatAContext ctx);
	/**
	 * Visit a parse tree produced by the {@code characterStringFormatS}
	 * labeled alternative in {@link SmallPearlParser#characterStringFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCharacterStringFormatS(SmallPearlParser.CharacterStringFormatSContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#channel}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitChannel(SmallPearlParser.ChannelContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#writeStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitWriteStatement(SmallPearlParser.WriteStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#readStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadStatement(SmallPearlParser.ReadStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#read_from}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRead_from(SmallPearlParser.Read_fromContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readAbsPosition}
	 * labeled alternative in {@link SmallPearlParser#readPosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadAbsPosition(SmallPearlParser.ReadAbsPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readRelPosition}
	 * labeled alternative in {@link SmallPearlParser#readPosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadRelPosition(SmallPearlParser.ReadRelPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readRSTPosition}
	 * labeled alternative in {@link SmallPearlParser#readPosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadRSTPosition(SmallPearlParser.ReadRSTPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code writeAbsPosition}
	 * labeled alternative in {@link SmallPearlParser#writePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitWriteAbsPosition(SmallPearlParser.WriteAbsPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code writeRelPosition}
	 * labeled alternative in {@link SmallPearlParser#writePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitWriteRelPosition(SmallPearlParser.WriteRelPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code writeRSTPosition}
	 * labeled alternative in {@link SmallPearlParser#writePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitWriteRSTPosition(SmallPearlParser.WriteRSTPositionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ReadWriteAbsolutePositionCOL}
	 * labeled alternative in {@link SmallPearlParser#readWriteAbsolutePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteAbsolutePositionCOL(SmallPearlParser.ReadWriteAbsolutePositionCOLContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ReadWriteAbsolutePositionLINE}
	 * labeled alternative in {@link SmallPearlParser#readWriteAbsolutePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteAbsolutePositionLINE(SmallPearlParser.ReadWriteAbsolutePositionLINEContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ReadWriteAbsolutePositionPOS}
	 * labeled alternative in {@link SmallPearlParser#readWriteAbsolutePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteAbsolutePositionPOS(SmallPearlParser.ReadWriteAbsolutePositionPOSContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ReadWriteAbsolutePositionSOP}
	 * labeled alternative in {@link SmallPearlParser#readWriteAbsolutePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteAbsolutePositionSOP(SmallPearlParser.ReadWriteAbsolutePositionSOPContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readWriteRelativePositionX}
	 * labeled alternative in {@link SmallPearlParser#readWriteRelativePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteRelativePositionX(SmallPearlParser.ReadWriteRelativePositionXContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readWriteRelativePositionSKIP}
	 * labeled alternative in {@link SmallPearlParser#readWriteRelativePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteRelativePositionSKIP(SmallPearlParser.ReadWriteRelativePositionSKIPContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readWriteRelativePositionPAGE}
	 * labeled alternative in {@link SmallPearlParser#readWriteRelativePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteRelativePositionPAGE(SmallPearlParser.ReadWriteRelativePositionPAGEContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readWriteRelativePositionADV}
	 * labeled alternative in {@link SmallPearlParser#readWriteRelativePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteRelativePositionADV(SmallPearlParser.ReadWriteRelativePositionADVContext ctx);
	/**
	 * Visit a parse tree produced by the {@code readWriteRelativePositionEOF}
	 * labeled alternative in {@link SmallPearlParser#readWriteRelativePosition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReadWriteRelativePositionEOF(SmallPearlParser.ReadWriteRelativePositionEOFContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#takeStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTakeStatement(SmallPearlParser.TakeStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#takeFrom}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTakeFrom(SmallPearlParser.TakeFromContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#sendStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSendStatement(SmallPearlParser.SendStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#sendTo}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSendTo(SmallPearlParser.SendToContext ctx);
	/**
	 * Visit a parse tree produced by the {@code take_send_rst_s_ctrl_format_RST}
	 * labeled alternative in {@link SmallPearlParser#take_send_rst_s_ctrl_format}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTake_send_rst_s_ctrl_format_RST(SmallPearlParser.Take_send_rst_s_ctrl_format_RSTContext ctx);
	/**
	 * Visit a parse tree produced by the {@code take_send_rst_s_ctrl_format_S}
	 * labeled alternative in {@link SmallPearlParser#take_send_rst_s_ctrl_format}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTake_send_rst_s_ctrl_format_S(SmallPearlParser.Take_send_rst_s_ctrl_format_SContext ctx);
	/**
	 * Visit a parse tree produced by the {@code take_send_rst_s_ctrl_format_CONTROL}
	 * labeled alternative in {@link SmallPearlParser#take_send_rst_s_ctrl_format}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTake_send_rst_s_ctrl_format_CONTROL(SmallPearlParser.Take_send_rst_s_ctrl_format_CONTROLContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType(SmallPearlParser.TypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#simple_type}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSimple_type(SmallPearlParser.Simple_typeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeTime}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeTime(SmallPearlParser.TypeTimeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_char}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_char(SmallPearlParser.Type_charContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_fixed}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_fixed(SmallPearlParser.Type_fixedContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_float}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_float(SmallPearlParser.Type_floatContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_duration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_duration(SmallPearlParser.Type_durationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_clock}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_clock(SmallPearlParser.Type_clockContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_bit}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_bit(SmallPearlParser.Type_bitContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#type_realtime_object}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType_realtime_object(SmallPearlParser.Type_realtime_objectContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#interruptSpecification}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitInterruptSpecification(SmallPearlParser.InterruptSpecificationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#dationSpecification}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDationSpecification(SmallPearlParser.DationSpecificationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#specifyTypeDation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSpecifyTypeDation(SmallPearlParser.SpecifyTypeDationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#dationDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDationDeclaration(SmallPearlParser.DationDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typeDation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeDation(SmallPearlParser.TypeDationContext ctx);
	/**
	 * Visit a parse tree produced by the {@code sourceSinkAttributeIN}
	 * labeled alternative in {@link SmallPearlParser#sourceSinkAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSourceSinkAttributeIN(SmallPearlParser.SourceSinkAttributeINContext ctx);
	/**
	 * Visit a parse tree produced by the {@code sourceSinkAttributeOUT}
	 * labeled alternative in {@link SmallPearlParser#sourceSinkAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSourceSinkAttributeOUT(SmallPearlParser.SourceSinkAttributeOUTContext ctx);
	/**
	 * Visit a parse tree produced by the {@code sourceSinkAttributeINOUT}
	 * labeled alternative in {@link SmallPearlParser#sourceSinkAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSourceSinkAttributeINOUT(SmallPearlParser.SourceSinkAttributeINOUTContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#systemDation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSystemDation(SmallPearlParser.SystemDationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#classAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClassAttribute(SmallPearlParser.ClassAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#alphicDation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAlphicDation(SmallPearlParser.AlphicDationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#basicDation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBasicDation(SmallPearlParser.BasicDationContext ctx);
	/**
	 * Visit a parse tree produced by the {@code typeOfTransmissionDataALL}
	 * labeled alternative in {@link SmallPearlParser#typeOfTransmissionData}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeOfTransmissionDataALL(SmallPearlParser.TypeOfTransmissionDataALLContext ctx);
	/**
	 * Visit a parse tree produced by the {@code typeOfTransmissionDataSimpleType}
	 * labeled alternative in {@link SmallPearlParser#typeOfTransmissionData}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeOfTransmissionDataSimpleType(SmallPearlParser.TypeOfTransmissionDataSimpleTypeContext ctx);
	/**
	 * Visit a parse tree produced by the {@code typeOfTransmissionDataCompoundType}
	 * labeled alternative in {@link SmallPearlParser#typeOfTransmissionData}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypeOfTransmissionDataCompoundType(SmallPearlParser.TypeOfTransmissionDataCompoundTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#accessAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAccessAttribute(SmallPearlParser.AccessAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typology}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypology(SmallPearlParser.TypologyContext ctx);
	/**
	 * Visit a parse tree produced by the {@code dimension1Star}
	 * labeled alternative in {@link SmallPearlParser#dimension1}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDimension1Star(SmallPearlParser.Dimension1StarContext ctx);
	/**
	 * Visit a parse tree produced by the {@code dimension1Integer}
	 * labeled alternative in {@link SmallPearlParser#dimension1}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDimension1Integer(SmallPearlParser.Dimension1IntegerContext ctx);
	/**
	 * Visit a parse tree produced by the {@code dimension2Integer}
	 * labeled alternative in {@link SmallPearlParser#dimension2}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDimension2Integer(SmallPearlParser.Dimension2IntegerContext ctx);
	/**
	 * Visit a parse tree produced by the {@code dimension3Integer}
	 * labeled alternative in {@link SmallPearlParser#dimension3}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDimension3Integer(SmallPearlParser.Dimension3IntegerContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#typologyAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTypologyAttribute(SmallPearlParser.TypologyAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#dimensionAttribute}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDimensionAttribute(SmallPearlParser.DimensionAttributeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#boundaryDenotation}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBoundaryDenotation(SmallPearlParser.BoundaryDenotationContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#indices}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIndices(SmallPearlParser.IndicesContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#compoundType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCompoundType(SmallPearlParser.CompoundTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#ioStructure}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIoStructure(SmallPearlParser.IoStructureContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#ioStructureComponent}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIoStructureComponent(SmallPearlParser.IoStructureComponentContext ctx);
	/**
	 * Visit a parse tree produced by the {@code cosExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCosExpression(SmallPearlParser.CosExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code additiveExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAdditiveExpression(SmallPearlParser.AdditiveExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code expExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExpExpression(SmallPearlParser.ExpExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code TOFIXEDExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTOFIXEDExpression(SmallPearlParser.TOFIXEDExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code unaryMultiplicativeExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnaryMultiplicativeExpression(SmallPearlParser.UnaryMultiplicativeExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code divideIntegerExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDivideIntegerExpression(SmallPearlParser.DivideIntegerExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code unarySubtractiveExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnarySubtractiveExpression(SmallPearlParser.UnarySubtractiveExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code notExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNotExpression(SmallPearlParser.NotExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code sizeofExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSizeofExpression(SmallPearlParser.SizeofExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code OrExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOrExpression(SmallPearlParser.OrExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code eqRelationalExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEqRelationalExpression(SmallPearlParser.EqRelationalExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code entierExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEntierExpression(SmallPearlParser.EntierExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code subtractiveExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSubtractiveExpression(SmallPearlParser.SubtractiveExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code atanExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAtanExpression(SmallPearlParser.AtanExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AndExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAndExpression(SmallPearlParser.AndExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code taskFunction}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTaskFunction(SmallPearlParser.TaskFunctionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code gtRelationalExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGtRelationalExpression(SmallPearlParser.GtRelationalExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code nowFunction}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNowFunction(SmallPearlParser.NowFunctionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code geRelationalExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGeRelationalExpression(SmallPearlParser.GeRelationalExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code sqrtExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSqrtExpression(SmallPearlParser.SqrtExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code tanExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTanExpression(SmallPearlParser.TanExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code CONTExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCONTExpression(SmallPearlParser.CONTExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code sinExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSinExpression(SmallPearlParser.SinExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code absExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAbsExpression(SmallPearlParser.AbsExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code leRelationalExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLeRelationalExpression(SmallPearlParser.LeRelationalExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code dateFunction}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDateFunction(SmallPearlParser.DateFunctionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code neRelationalExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNeRelationalExpression(SmallPearlParser.NeRelationalExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code signExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSignExpression(SmallPearlParser.SignExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ltRelationalExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLtRelationalExpression(SmallPearlParser.LtRelationalExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code TOFLOATExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTOFLOATExpression(SmallPearlParser.TOFLOATExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code shiftExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitShiftExpression(SmallPearlParser.ShiftExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code unaryAdditiveExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnaryAdditiveExpression(SmallPearlParser.UnaryAdditiveExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code remainderExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRemainderExpression(SmallPearlParser.RemainderExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code catExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCatExpression(SmallPearlParser.CatExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code TOCHARExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTOCHARExpression(SmallPearlParser.TOCHARExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code baseExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBaseExpression(SmallPearlParser.BaseExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code multiplicativeExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMultiplicativeExpression(SmallPearlParser.MultiplicativeExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code divideExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDivideExpression(SmallPearlParser.DivideExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code tanhExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTanhExpression(SmallPearlParser.TanhExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code cshiftExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCshiftExpression(SmallPearlParser.CshiftExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ExorExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExorExpression(SmallPearlParser.ExorExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code unarySignedLiteralExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnarySignedLiteralExpression(SmallPearlParser.UnarySignedLiteralExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code TOBITExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTOBITExpression(SmallPearlParser.TOBITExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code fitExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFitExpression(SmallPearlParser.FitExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code exponentiationExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExponentiationExpression(SmallPearlParser.ExponentiationExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code roundExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRoundExpression(SmallPearlParser.RoundExpressionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code lnExpression}
	 * labeled alternative in {@link SmallPearlParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLnExpression(SmallPearlParser.LnExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#unaryLiteralExpression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnaryLiteralExpression(SmallPearlParser.UnaryLiteralExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#unaryExpression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnaryExpression(SmallPearlParser.UnaryExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#expressionList}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExpressionList(SmallPearlParser.ExpressionListContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#numericLiteral}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNumericLiteral(SmallPearlParser.NumericLiteralContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#numericLiteralUnsigned}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNumericLiteralUnsigned(SmallPearlParser.NumericLiteralUnsignedContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#numericLiteralPositive}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNumericLiteralPositive(SmallPearlParser.NumericLiteralPositiveContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#numericLiteralNegative}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNumericLiteralNegative(SmallPearlParser.NumericLiteralNegativeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#primaryExpression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPrimaryExpression(SmallPearlParser.PrimaryExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#constantExpression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantExpression(SmallPearlParser.ConstantExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#constantFixedExpression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantFixedExpression(SmallPearlParser.ConstantFixedExpressionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#additiveConstantFixedExpressionTerm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAdditiveConstantFixedExpressionTerm(SmallPearlParser.AdditiveConstantFixedExpressionTermContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#subtractiveConstantFixedExpressionTerm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSubtractiveConstantFixedExpressionTerm(SmallPearlParser.SubtractiveConstantFixedExpressionTermContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#constantFixedExpressionTerm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantFixedExpressionTerm(SmallPearlParser.ConstantFixedExpressionTermContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#multiplicationConstantFixedExpressionTerm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMultiplicationConstantFixedExpressionTerm(SmallPearlParser.MultiplicationConstantFixedExpressionTermContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#divisionConstantFixedExpressionTerm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDivisionConstantFixedExpressionTerm(SmallPearlParser.DivisionConstantFixedExpressionTermContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#remainderConstantFixedExpressionTerm}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRemainderConstantFixedExpressionTerm(SmallPearlParser.RemainderConstantFixedExpressionTermContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#constantFixedExpressionFactor}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantFixedExpressionFactor(SmallPearlParser.ConstantFixedExpressionFactorContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#constantFixedExpressionFit}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstantFixedExpressionFit(SmallPearlParser.ConstantFixedExpressionFitContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#convertStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConvertStatement(SmallPearlParser.ConvertStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#convertToStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConvertToStatement(SmallPearlParser.ConvertToStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#convertFromStatement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConvertFromStatement(SmallPearlParser.ConvertFromStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#formatOrPositionConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormatOrPositionConvert(SmallPearlParser.FormatOrPositionConvertContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#formatFactorConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormatFactorConvert(SmallPearlParser.FormatFactorConvertContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#formatConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormatConvert(SmallPearlParser.FormatConvertContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#listFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitListFormat(SmallPearlParser.ListFormatContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#rFormat}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRFormat(SmallPearlParser.RFormatContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionConvertRST}
	 * labeled alternative in {@link SmallPearlParser#positionConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionConvertRST(SmallPearlParser.PositionConvertRSTContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionConvertX}
	 * labeled alternative in {@link SmallPearlParser#positionConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionConvertX(SmallPearlParser.PositionConvertXContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionConvertPOS}
	 * labeled alternative in {@link SmallPearlParser#positionConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionConvertPOS(SmallPearlParser.PositionConvertPOSContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionConvertADV}
	 * labeled alternative in {@link SmallPearlParser#positionConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionConvertADV(SmallPearlParser.PositionConvertADVContext ctx);
	/**
	 * Visit a parse tree produced by the {@code positionConvertSOP}
	 * labeled alternative in {@link SmallPearlParser#positionConvert}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPositionConvertSOP(SmallPearlParser.PositionConvertSOPContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#stringSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStringSlice(SmallPearlParser.StringSliceContext ctx);
	/**
	 * Visit a parse tree produced by the {@code case1BitSlice}
	 * labeled alternative in {@link SmallPearlParser#bitSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase1BitSlice(SmallPearlParser.Case1BitSliceContext ctx);
	/**
	 * Visit a parse tree produced by the {@code case2BitSlice}
	 * labeled alternative in {@link SmallPearlParser#bitSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase2BitSlice(SmallPearlParser.Case2BitSliceContext ctx);
	/**
	 * Visit a parse tree produced by the {@code case1CharSlice}
	 * labeled alternative in {@link SmallPearlParser#charSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase1CharSlice(SmallPearlParser.Case1CharSliceContext ctx);
	/**
	 * Visit a parse tree produced by the {@code case2CharSlice}
	 * labeled alternative in {@link SmallPearlParser#charSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase2CharSlice(SmallPearlParser.Case2CharSliceContext ctx);
	/**
	 * Visit a parse tree produced by the {@code case3CharSlice}
	 * labeled alternative in {@link SmallPearlParser#charSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase3CharSlice(SmallPearlParser.Case3CharSliceContext ctx);
	/**
	 * Visit a parse tree produced by the {@code case4CharSlice}
	 * labeled alternative in {@link SmallPearlParser#charSlice}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCase4CharSlice(SmallPearlParser.Case4CharSliceContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#literal}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLiteral(SmallPearlParser.LiteralContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#constant}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstant(SmallPearlParser.ConstantContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#bitStringConstant}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBitStringConstant(SmallPearlParser.BitStringConstantContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#timeConstant}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTimeConstant(SmallPearlParser.TimeConstantContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#durationConstant}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDurationConstant(SmallPearlParser.DurationConstantContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#hours}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitHours(SmallPearlParser.HoursContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#minutes}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMinutes(SmallPearlParser.MinutesContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#seconds}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSeconds(SmallPearlParser.SecondsContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#cpp_inline}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCpp_inline(SmallPearlParser.Cpp_inlineContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#lengthDefinition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLengthDefinition(SmallPearlParser.LengthDefinitionContext ctx);
	/**
	 * Visit a parse tree produced by the {@code lengthDefinitionFixedType}
	 * labeled alternative in {@link SmallPearlParser#lengthDefinitionType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLengthDefinitionFixedType(SmallPearlParser.LengthDefinitionFixedTypeContext ctx);
	/**
	 * Visit a parse tree produced by the {@code lengthDefinitionFloatType}
	 * labeled alternative in {@link SmallPearlParser#lengthDefinitionType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLengthDefinitionFloatType(SmallPearlParser.LengthDefinitionFloatTypeContext ctx);
	/**
	 * Visit a parse tree produced by the {@code lengthDefinitionBitType}
	 * labeled alternative in {@link SmallPearlParser#lengthDefinitionType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLengthDefinitionBitType(SmallPearlParser.LengthDefinitionBitTypeContext ctx);
	/**
	 * Visit a parse tree produced by the {@code lengthDefinitionCharacterType}
	 * labeled alternative in {@link SmallPearlParser#lengthDefinitionType}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLengthDefinitionCharacterType(SmallPearlParser.LengthDefinitionCharacterTypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#precision}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPrecision(SmallPearlParser.PrecisionContext ctx);
	/**
	 * Visit a parse tree produced by {@link SmallPearlParser#length}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLength(SmallPearlParser.LengthContext ctx);
}