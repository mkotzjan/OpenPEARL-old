all: imc.jar

SRCS = InterModuleChecker.java TargetXml.java ModuleXml.java ReadXml.java \
	SystemEntries.java SystemEntry.java Parameter.java Error.java \
	CodeGenerator.java
%.class: %.java
	javac $<

CLASSES = $(SRCS:.java=.class)

imc.jar:  $(CLASSES)
	jar -cf imc.jar $(CLASSES) 

InterModuleChecker.class: InterModuleChecker.java
	javac InterModuleChecker.java

clean:
	rm -f *.class
	rm -f imc.jar 
