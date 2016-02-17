First of all, thanks for your interest in Jamtaba! 

***

### Coding Style for Jamtaba
Jamtaba coding style is the same [Qt Coding Style](https://wiki.qt.io/Qt_Coding_Style). If you are using QtCreator as IDE you can use a plugin called **Beautifier** to format your code applying the Jamtaba coding style. This QtCreator plugin **MUST** be used with an external tool. Please [read](http://doc.qt.io/qtcreator/creator-beautifier.html) the details about how use this plugin and configure the external tool.

In Jamtaba we are using the external tool [Uncrustify](http://uncrustify.sourceforge.net/) combined with the Beautifier QtCreator plugin to format the code. You must configure QtCreator to use the *uncrustify config file* inside the Jamtaba project. In QtCretor go to **Tools -> Options -> Beautifier** and check the option **use file uncrustify.cfg defined in project files**.

**obs:** The file **uncrustify.cfg** is just a copy of this [file](https://gist.github.com/trollixx/11204806).

***

### Testing
See the **Client/tests** folder for some test examples. These tests are using the QtTest framework. This is an easy to use test tool for Qt applications and no additional setup/installing is required. A very good resource to learn how write these tests is the Qt official [Test Tutorial](http://doc.qt.io/qt-5/qtest-tutorial.html)
