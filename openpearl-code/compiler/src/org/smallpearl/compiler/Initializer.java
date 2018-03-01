package org.smallpearl.compiler;

import com.sun.corba.se.impl.orbutil.closure.Constant;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTreeProperty;
import org.smallpearl.compiler.SmallPearlParser.Unlabeled_statementContext;
import org.smallpearl.compiler.SymbolTable.ModuleEntry;

import java.util.LinkedList;

public class Initializer  {
	private ParserRuleContext m_context;
	private ConstantValue     m_constant;

	    public Initializer(ParserRuleContext ctx, ConstantValue constant) {
	    	m_context = ctx;
	    	m_constant = constant;
	    }

	public ParserRuleContext getContext() { return m_context; }
	public ConstantValue getConstant() { return m_constant; }
    public void setConstant(ConstantValue constant) { m_constant = constant; }

	public String toString() {
	 	if ( m_constant instanceof ConstantFixedValue) {
	 		return ((ConstantFixedValue)m_constant).toString();
		}
		else if ( m_constant instanceof ConstantFloatValue) {
            return ((ConstantFloatValue)m_constant).toString();
        }
        else if ( m_constant instanceof ConstantDurationValue) {
            return ((ConstantDurationValue)m_constant).toString();
        }
        else if ( m_constant instanceof ConstantClockValue) {
            return ((ConstantClockValue)m_constant).toString();
        }
        else if ( m_constant instanceof ConstantCharacterValue) {
            return ((ConstantCharacterValue)m_constant).toString();
        }
        else if ( m_constant instanceof ConstantBitValue) {
            return ((ConstantBitValue)m_constant).toString();
        }
        else {
			return m_constant.toString();
		}

//		return "???";
	}
}
