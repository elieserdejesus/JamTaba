package jamtaba.command;

/** @author zeh */
public class JapaneseUtils {

    public static boolean containJapaneseSymbols(String str){
        boolean containJapaneseSymbols = false;
        for (int i = 0; i < str.length(); i++) {
            int code = str.codePointAt(i);
            if(code >= 0x3000 && code <= 0x9faf){
                return true;
            }
        }
        return containJapaneseSymbols;
    }
    
    
    public static void main(String args[]){
        System.out.println(containJapaneseSymbols("圍"));
    }
    
}
