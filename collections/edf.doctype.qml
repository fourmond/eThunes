import DocTypes 1.0


Collection {
    name: "edf"

    description: "Factures EDF"

    DocType {
        name: "edf-bill"
        parent: "bill"
        function isMine(pdf) {
            print("2");
            print(pdf);
            for (var p in pdf) {
                print("Object item:", p, "=", pdf[p])
            }
            if(pdf["text"].match(/\.edf\.(com|fr)/)) {
                return 20;
            }
            return 0;
        }
            
    }
}
