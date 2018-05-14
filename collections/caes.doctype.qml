import DocTypes 1.0


Collection {
    name: "caes"

    description: "Factures du CAES du CNRS"

    DocType {
        name: "caes-bill"
        parent: "bill"

        infoFormat: "Facture CAES du %{date%date:dd/MM/yyyy}, %{amount%A}"
        
        function isMine(pdf) {
            if(pdf["text"].match(/CAES DU CNRS/)) {
                return 20;
            }
            return 0;
        }

        function parseMetaData(pdf) {
            var val = {};
            var re = new RegExp("Total\\s+Net\\s+(\\d+)\.(\\d+)", "i");
            var m;
            if(m = pdf["text-layout"].match(re)) {
                // print(m);
                var v = parseInt(m[1])*100 + parseInt(m[2]);
                val["amount"] = v;
            }
            if(m = pdf["text-layout"].match(/Date de Facture : (\d+)\/(\d+)\/(\d+)/i)) {
                val["date"] = new Date(m[3], parseInt(m[2])-1, m[1]);
            }
            if(m = pdf["text-layout"].match(/Identifiant\s*CAES\s*:\s*(\d+)/i)) {
                val["identifiant"] = m[1];
            }
            if(m = pdf["text-layout"].match(/FACTURE No:\s+(\S+)/i)) {
                val["numero"] = m[1];
            }
            return val;
        }
            
    }
}
