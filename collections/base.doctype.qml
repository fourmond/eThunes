import DocTypes 1.0


Collection {
    name: "default"

    description: "Documents that can be used as a base for all others, though it is absolutely not necessary"

    DocType {
        name: "dated-doc"
        dates: [ "date" ]
    }

    DocType {
        name: "bill"
        parent: "dated-doc"
        amounts: [ "amount" ]
    }

    DocType {
        name: "payslip"
        parent: "dated-doc"
        amounts: [ "amount" ]
    }
}
