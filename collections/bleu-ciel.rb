# -*- coding: utf-8 -*-
CollectionDefinition.collection('bleu-ciel') do

  public_name "EDF Bleu Ciel"
  description <<EOD
Factures EDF Bleu Ciel
EOD
  
  document 'facture' do
    public_name "Facture"
    display "Facture du %{date%date:d/MM/yyyy}: %{montant%A} (%{ref-client} -- %{facture})"
    format "Facture-%{date%date:yyyy-MM}.pdf"

    matcher 'prelevement', 'montant', 9

    def parse(doc)
      res = {}
      if doc['text'] =~ /Facture.*?\s+du\s+(#{Dates::FrenchFullDateRE})/i
        res['date'] = Dates::parse_french_date($1)
      elsif doc['text-layout'] =~ /Facture.*?\s+du\s+(#{Dates::DateRE})/i
        res['date'] = Dates::parse_french_date($1)
      end
      if doc['text'] =~ /N(.|..)\s*facture\s*:\s*(\S+)/i
        res['facture'] = $2.chomp
      end

      if doc['text'] =~ /Votre\s+facture\s+du\s+(\d+)\/(\d+)\/(\d+)\s+N\D+([\d\s]+)/
        res['date'] = Time.local($3.to_i, $2.to_i, $1.to_i)
        res['facture'] = $4.gsub(/\s/,'')
      end


      if doc['text'] =~ /N(.|..)\s*client\s*:\s*([\d\s]+)/i
        res['ref-client'] = $2.chomp
      end
      if doc['text-layout'] =~ /montant\s*\s*(?:non\s*)?\S+\s*(le\s*(\d+)\/(\d+)\/(\d+))?\s*([-\d]+)[.,](\d+).*?TTC/i 
        if $1
          res['prelevement'] = Time.local($4.to_i, $3.to_i, $2.to_i)
        else
          res['prelevement'] = res['date'] + 7 * 24 * 3600
        end
        units = $5.to_i * 100
        if units > 0
          units += $6.to_i
        else
          units -= $6.to_i
        end
        res['montant'] = units
      elsif doc['text'] =~ /le\s*montant\D+(\d+),(\d+).*le\s*(\d+)\/(\d+)\/(\d+)/
        res['montant'] = $1.to_i * 100 + $2.to_i
        res['prelevement'] = Time.local($5.to_i, $4.to_i, $3.to_i)
      end
      return res
    end
  end


  def fetch(fetcher, creds, docs)
    us = creds["login"]
    pw = creds["passwd"]

    url = "http://particuliers.edf.com/FRONT/NetExpress/dcc_legacy/france/tracking/specifique/collecteur.ajax.php?identifiant=" + us.gsub(/@/, "%40")

    p1 = fetcher.get(url)

    params = {
      'login' => us,
      'password' => pw,
      'validate' => 'OK'
    }

    p2 = fetcher.post("https://particuliers.edf.com/FRONT/NetExpress/services/serviceRedirectionAel.php?action=post&marche=part", params)

    tg, frm = p2.form_data

    p3 = fetcher.post(tg, frm)
    tg, frm = p3.form_data
    p4 = fetcher.post(tg, frm)
    
    # Now, we look at the bills !

    base = fetcher.get('https://monagencepart.edf.fr/ASPFront/appmanager/ASPFront/front?_nfpb=true&_pageLabel=private/page_mes_factures&portletInstance2=portlet_suivi_consommation_2')

    pdfs = []
    years = [nil]
    years += base.linked_hrefs(".years a")
    # p years

    for y in years
      base = fetcher.get(y) if y
      pdfs.concat(base.linked_hrefs("a.pdf"))
    end

    # p pdfs

    for f in pdfs
      # Check !
      doc = fetcher.get(f)
      fetcher.add_document(doc, "facture")
    end
    
    
  end
end
