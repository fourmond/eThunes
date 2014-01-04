# -*- coding: utf-8 -*-
CollectionDefinition.collection('impots') do

  public_name "Impôts"
  description <<EOD
Gestion des impots et autres taxes que l'on paie en france, et qui
sont accessibles sur le site http://impots.gouv.fr
EOD

  document 'avis-imposition' do
    public_name "Avis d'imposition"
    display "Avis d'imposition %{annee} (revenus de %{annee-concernee}), numéro fiscal %{numero-fiscal}"
    format 'Impots/Avis-imposition-%{numero-fiscal}-%{annee}.pdf'

    def parse(doc)
      res = {}
      if doc['text'] =~ /revenus\s*de\s*l.*ann.*(\d{4})/
        # Cas 2009
        res['annee-concernee'] = $1
        if doc['text'] =~ /Num[^:]+cal\s*(?:\(vous\))?\s*:\s*(\d+)/ ||
            doc['text-layout'] =~ /Num[^:]+cal\s*(?:\(vous\))?\s*:\s*(\d+)/
          res['numero-fiscal'] = $1
        end
      elsif doc['text'] =~ /^DE\s*(\d{4})/
        res['annee-concernee'] = $1
        if doc['text'] =~ /N u m [^:]* r o f i s c a l : (\d+)/
          res['numero-fiscal'] = $1
        end
      elsif doc['text'] =~ /^SUR\s*LE\s*REVENU\s*(\d{4})/
        # 2010
        res['annee-concernee'] = ($1.to_i - 1).to_s
        if doc['text-layout'] =~ /Num[^:]+scal\s*(?:\(vous\))?\s*:\s*(\d+)/
          res['numero-fiscal'] = $1
        end
      end
      if res['annee-concernee']
        res['annee'] = (res['annee-concernee'].to_i + 1).to_s
        if doc['text-layout'] =~ 
            /mise\s+en\s+recouvrement\s*:\s*(\d+)\/(\d+)\/(\d+)/
          res['recouvrement'] = Time.local($3.to_i, $2.to_i, $1.to_i)
        end
        if doc['text-layout'] =~ 
            /limite\s+de\s+paiement\s*(?::\s*)?(\d+)\/(\d+)\/(\d+)/
          res['paiment'] = Time.local($3.to_i, $2.to_i, $1.to_i)
          
        end
        if doc['text-layout'] =~ 
            /Montant de votre imp.*t \(en euros\)\s+(\d+)/
          res['montant'] = $1.to_i*100
        end
        if doc['text-layout'] =~ 
            /Montant de la restitution \(en euros\)\s+(\d+)/
          res['montant'] = -($1.to_i*100)
        end
        if ! res.key?('montant') && 
            doc['text-layout'] =~
            /total de votre imposition nette [^=]+ recouvrer est de\s*=\s*(\d+)/
          # 2008 et avant ?
          res['montant'] = $1.to_i*100
        end
      end
      return res
    end
  end

  document 'taxe-habitation' do
    public_name "Taxe d'habitation"
    display "Taxe d'habitation %{annee}, numéro fiscal %{numero-fiscal}"
    format "Impots/Taxe-habitation-%{numero-fiscal}-%{annee}.pdf"

    def parse(doc)
      res = {}
      if doc['text-layout'] =~ 
          /mise\s+en\s+recouvrement\s*:\s*(\d+)\/(\d+)\/(\d+)/
        annee = $3
        if annee.to_i < 1900
          annee = annee.to_i
          if annee > 20
            annee = annee + 1900
          else
            annee = annee + 2000
          end
          annee = annee.to_s
        end
        res['annee'] = annee
        res['recouvrement'] = Time.local(annee.to_i, $2.to_i, $1.to_i)
      end
      if doc['text-layout'] =~ 
          /limite\s+de\s+paiement\s*:\s*(\d+)\/(\d+)\/(\d+)/
        res['paiment'] = Time.local($3.to_i, $2.to_i, $1.to_i)
      end
      if doc['text-layout'] =~ /Num[^:]+scal\s*(?:\(vous\))?\s*:\s*(\d+)/
        res['numero-fiscal'] = $1
      elsif doc['text-layout'] =~ /fiscal\s*:\s*([\d\s]+)/
        num = $1
        res['numero-fiscal'] = num.gsub(/\s+/,'')
      end
      if doc['text-layout'] =~ 
          /Somme\s+totale.*payer\s+\(en\s+euros\)\s+(\d+)/
        res['montant'] = $1.to_i*100
      elsif doc['text'] =~ 
          /de\s+votre\s+imp.{1,5}t[\n\s]+(\d+)/
        res['montant'] = $1.to_i*100
      end
      return res
    end
  end

  document "accuse-reception" do
    display "Accusé de réception de déclaration %{annee} (revenus de %{annee-concernee}), numéro fiscal %{numero-fiscal}"
    format "Impots/Accuse-reception-%{numero-fiscal}-%{annee}.pdf"
    public_name "Accusé de réception de déclaration (signés)"
    
    def parse(doc)
      res = {}
      if doc['text'] =~ /claration\s*des\s*revenus\s*(\d{4})/
        res['annee-concernee'] = $1
      end
      if doc['text'] =~ /fiscal\s*du\s*soumissionnaire\s*:\s*(\d+)/
        res['numero-fiscal'] = $1
      end
      if res['annee-concernee']
        res['annee'] = (res['annee-concernee'].to_i + 1).to_s
      end
      if doc['text'] =~ /Date\s*de\s*d[^p]+p[^t]+t\s*:\s*(\d+)\s*(#{Dates::FrenchMonthRE})\s*(\d+)\D+(\d+):(\d+):(\d+)/i
        res['date-declaration'] = 
          Time.local($3.to_i, Dates::parse_french_month($2), $1.to_i, 
                     $4.to_i, $5.to_i, $6.to_i)
      end
      return res
    end
  end

  document "declaration" do
    public_name "Déclaration de revenus"
    display "Déclaration %{annee} (revenus de %{annee-concernee}), numéro fiscal %{numero-fiscal}"
    format "Impots/Declaration-%{numero-fiscal}-%{annee}.pdf"

    def self.parse(doc)
      res = {}
      if doc['text'] =~ /CLARATION\s*DES\s*REVENUS\s*(\d{4})/
        res['annee-concernee'] = $1
      end
      if doc['text'] =~ /^(\d{13})\s*C/
        res['numero-fiscal'] = $1
      end
      if res['annee-concernee']
        res['annee'] = (res['annee-concernee'].to_i + 1).to_s
      end
      return res
    end
  end
end
