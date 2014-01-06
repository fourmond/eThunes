# -*- coding: utf-8 -*-
CollectionDefinition.collection('assedic') do
  public_name 'Assedic'
  description <<EOD
Documents afférents aux assedics
EOD

  document 'avis' do 
    display "Avis de %{mois%date:MM/yyyy} pour %{identifiant}"
    format "Avis-%{identifiant}-%{mois%date:yyyy-MM}.pdf"
    public_name "Déclaration de situation"

    def parse(doc)
      res = {}
      if doc['text'] =~ /Identifiant\s*:\s*(\S+)/
        res['identifiant'] = $1
      end
      if doc['text'] =~ /mois\s+d(?:e\s|')\s*(\S+)\s+(\d+)/
        res['mois'] = Time.local($2.to_i, Dates::parse_french_month($1))
      end
      if doc['text'] =~ /le\s+(\d+)\/(\d+)\/(\d+)\s+\S+\s+(\d+):(\d+)/
        res['enregistrement'] = Time.local($3.to_i + 2000, $2.to_i, $1.to_i, $4.to_i, $5.to_i)
      end
      return res
    end
  end

  document "paiment" do

    display 'Paiment du %{date%date:dd/MM/yyyy} pour %{identifiant} : %{montant%A}'
    format 'Paiment-%{identifiant}-%{date%date:yyyy-MM}.pdf'
    public_name 'Avis de paiment'

    matcher 'date', 'montant', 5

    def parse(doc)
      res = {}
      if doc['text'] =~ /Identifiant\s*:\s*(\S+)/
        res['identifiant'] = $1
      end
      if doc['text'] =~ %r[REGLEMENT\s*DU\s*(\d+)/(\d+)/(\d+)]i
        res['date'] = Time.local($3.to_i, $2.to_i, $1.to_i)
      end
      if doc['text'] =~ /R..?glement\s*de\s*(\d+),(\d+)/i
        res['montant'] = $1.to_i * 100 + $2.to_i
      end
      return res
    end
  end
end
