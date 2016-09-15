all: imc.jar

imc.jar: InterModuleChecker.class
	jar -cf imc.jar *.class

InterModuleChecker.class: InterModuleChecker.java
	javac InterModuleChecker.java

clean:
	rm -f *.class
	rm -f imc.jar 
