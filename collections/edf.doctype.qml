import DocTypes 1.0


Collection {
    name: "edf"

    description: "Factures EDF"

    DocType {
        name: "edf-bill"
        parent: "bill"

        infoFormat: "Facture EDF du %{date%date:dd/MM/yyyy}, %{amount%A}"
        
        function isMine(pdf) {
            if(pdf["text"].match(/\.edf\.(com|fr)/)) {
                return 20;
            }
            return 0;
        }

        function parseMetaData(pdf) {
            var val = {};
            var re = new RegExp("facture du\\s+(\\d+)\\s+(" + frenchMonthsRE + ")\\s+(\\d+)", "i");
            var m;
            if(m = pdf["text"].match(re)) {
                // print(m);
                var mnth = parseFrenchMonth(m[2]);
                val["date"] = new Date(m[3], mnth, m[1]);
            }
            if(m = pdf["text-layout"].match(/montant\s+(non\s+)?pr..?lev..?\s+(-?)(\d+),(\d+)/i)) {
                // print(m);
                var v = parseInt(m[3])*100 + parseInt(m[4]);
                if(m[2] == "-") {
                    v = -v;
                }
                val["amount"] = v;
            }
            return val;
        }
            
    }
}
