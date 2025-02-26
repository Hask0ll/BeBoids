# BeBoids

## Concepts clés à comprendre
L'algorithme des Boids repose sur trois règles fondamentales qui permettent de simuler un comportement de groupe naturel :

**Séparation** : Éviter les collisions avec les autres boids voisins

**Alignement** : S'orienter dans la même direction que les boids voisins

**Cohésion** : Se diriger vers le centre du groupe de boids voisins

## Implémentation

### Boids manager
Ajoutez le Blueprint Boids Manager dans votre scene, puis paramètrez dans detail panel:

``NumBoids`` (Nombre de boids que vous voulez faire apparître)

``SpawnVolume`` (Déterminez la zone dans la qu'elle les Boids vont apparaître)

``BoidClass`` (Ajoutez en référence le BP_Boids)
