select autor, MAX(Wynik_Publikacyjny) Wynik_Publikacyjny from (SELECT temp.autor, SUM(temp.wynik)  OVER (
    partition by temp.Autor
    order by temp.Autor, temp.wynik
    ROWS between 3 PRECEDING and CURRENT ROW
) as Wynik_Publikacyjny
FROM (select A.Autor, (B.Punkty/B.LiczbaAutorow) wynik
 from Autorstwo A
  left join Praca B on A.Praca = B.ID
order by A.autor, B.Punkty/B.LiczbaAutorow desc) temp
order by autor, Wynik_Publikacyjny)
group by autor;









