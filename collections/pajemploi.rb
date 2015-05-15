# -*- coding: utf-8 -*-
CollectionDefinition.collection('pajemploi') do

  public_name "Pajemploi"
  description <<EOD
Différents documents afférents à Pajemploi (du côté assistantes maternelles)
EOD
  
  document 'payslip' do
    public_name "Bulletin de paie"
    display 'Bulletin de paie du %{month%date:MM/yyyy}: %{salary%A} %{fees%A}(%{employer})'
    format 'Bulletin-%{employer}-%{month%date:yyyy-MM}.pdf'

    matcher 'month', 'total', 20

    def parse(doc)
      res = {}
      if doc['text-layout'] =~ /du\s*:?\s*(\d+)\/(\d+)\/(\d+)\s*au\s*:?\s*/i
        res['month'] = Time.local($3.to_i, $2.to_i, $1.to_i)
      end
      if doc['text-layout'] =~ /Employeur\s*:?\s*(\w+)/
        res['employer'] = $1
      end
      if doc['text-layout'] =~ /jours?\s+d'activit.{1,3}\s*(\d+)/i
        res['days'] = $1.to_i
      end
      if doc['text-layout'] =~ /heures\s+normales?\s*(\d+)/i
        res['hours'] = $1.to_i
      end
      if doc['text-layout'] =~ /Salaire\s+net\s*(\d+),(\d+)/i
        res['salary'] = $1.to_i * 100 + $2.to_i
      end
      if doc['text-layout'] =~ /Indemni.*entretien\s+\s*(\d+),(\d+)/i
        res['fees'] = $1.to_i * 100 + $2.to_i
      end
      if doc['text-layout'] =~ /Volet\s+social.*?(\d+\S+)/i
        res['reference'] = $1
      end

      if res['salary'] && res['fees']
        res['total'] = res['salary'] + res['fees']
      end

      return res
    end
  end
    
  def fetch(fetcher, creds, docs)
    vals = {
      'j_username' => creds["login"],
      'j_password' => creds["passwd"],
      'j_passwordfake' => ''
    }
    login = fetcher.post("https://www.pajemploi.urssaf.fr/pajeweb/j_spring_security_check", vals)
    base = 0
    ps = []
    
    already_in = {}
    for d in docs
      if d['reference']
        already_in[d['reference']] = true
      end
    end
    
    while true
      a = {
        'order' => '',
        'nbStart' => "#{base}",
        'psdoSirt' => '',
        'choixDate' => 'emploi',
        'debutMoisPeriode' => '01',
        'debutAnneePeriode' => '2005',
        'finMoisPeriode' => '12',
        'finAnneePeriode' => Time.now.year.to_s,
        'typeActivite' => 'A'
      }
      stuff = fetcher.post("https://www.pajemploi.urssaf.fr/pajeweb/attesemploisala.jsp", a)
      begin
        nb = 0
        stuff.css('a').map do |lnk|
          if lnk['href'] && lnk['href']  =~ /\.pdf\?ref/
            ps << lnk['href']
            nb += 1
          end
        end
        base += nb
        if nb < 10
          break
        end
      rescue Exception => e
        p e
        break
      end
    end


    # Now, we loop over the files to download them !
    for f in ps 
      # Find out if in docs already.
      f =~ /ref=(.*)/
      ref = $1
      if ! already_in[ref]
        doc = fetcher.get("https://www.pajemploi.urssaf.fr#{f}")
        fetcher.add_document(doc, "payslip")
      end
    end
  end
end
