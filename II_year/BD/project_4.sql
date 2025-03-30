SELECT distinct A.Nazwisko, B.Liczba_Michała_Pilipczuka FROM Autor A LEFT JOIN 
        (SELECT autor_colleague, MIN(poziom) - 1 as Liczba_Michała_Pilipczuka
            FROM (
                SELECT distinct autor_prior, autor_colleague, LEVEL as poziom
                FROM 
                    (select distinct A.Autor as autor_prior, B.Autor as autor_colleague 
                    from Autorstwo A
                    join Autorstwo B
                    on A.Praca = B.Praca 
                    )
                START WITH autor_prior = 'Pilipczuk Mi' and autor_colleague = 'Pilipczuk Mi'
                CONNECT BY NOCYCLE PRIOR autor_colleague = autor_prior and autor_colleague != autor_prior
            )
        GROUP BY autor_colleague
        ORDER BY Liczba_Michała_Pilipczuka) B 
    ON A.Nazwisko = B.autor_colleague
order by Liczba_Michała_Pilipczuka;
