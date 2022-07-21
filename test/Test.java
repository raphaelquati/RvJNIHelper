public class Test {
    void m() {
        System.out.println("m invoked()!");
        System.out.flush();
    }

    int sum(int a, int b) {
        return a + b;
    }

    void printName(String name) {
        System.out.println("printName:" + name);
        System.out.flush();
    }

    String returnName(int a) {
        return "returnName(" + a + ")";
    }

    void makeException() throws Exception {
        throw new Exception("Test exception!");
    }


}

