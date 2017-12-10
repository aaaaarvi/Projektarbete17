
a=401
for i in dataPat_*.csv; do
  new=$(printf "dataPat_%d.csv" "$a") #04 pad to length of 4
  mv -i -- "$i" "$new"
  let a=a+1
done

a=401
for i in dataMom_*.csv; do
  new=$(printf "dataMom_%d.csv" "$a") #04 pad to length of 4
  mv -i -- "$i" "$new"
  let a=a+1
done

a=401
for i in dataTSMom_*.csv; do
  new=$(printf "dataTSMom_%d.csv" "$a") #04 pad to length of 4
  mv -i -- "$i" "$new"
  let a=a+1
done
