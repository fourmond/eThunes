# -*- coding: utf-8 -*-
CollectionDefinition.collection('base') do
  public_name 'General collection for general documents'
  description <<EOD
...
EOD

  document 'invoice' do

    attributes :amount, :date, :who
    
    display "Bill from %{who}, dating from %{date%date}: %{amount%A}"
    format "Bill-%{who}-%{date%date:yyyy-MM}.pdf"
    public_name "Bill"

  end

  document 'payslip' do

    attributes :amount, :date, :who
    
    display "Bill from %{who}, dating from %{date%date}: %{amount%A}"
    format "Bill-%{who}-%{date%date:yyyy-MM}.pdf"
    public_name "Payslip"

  end

end
