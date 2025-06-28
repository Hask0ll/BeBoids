# Simulation de Boids - Unreal Engine

Une implémentation complète de l'algorithme de Craig Reynolds (1986) pour simuler le comportement de nuées d'oiseaux dans Unreal Engine, avec optimisations avancées et systèmes d'évitement d'obstacles.

## 🐦 Aperçu du Projet

Ce projet reproduit fidèlement l'algorithme des boids présenté à SIGGRAPH 1987, en ajoutant des optimisations modernes et des fonctionnalités avancées pour des simulations performantes avec des milliers d'entités.

### ✨ Fonctionnalités Principales

- **Algorithme de Reynolds authentique** avec les 3 règles fondamentales
- **Optimisation par grille spatiale** pour des performances O(n) au lieu de O(n²)
- **Évitement d'obstacles sophistiqué** avec raycasting multi-directionnel
- **Field of view réaliste** pour un comportement naturel
- **Gestion de milliers de boids** en temps réel
- **Intégration FPS complète** pour interaction et observation

## 🧠 Fondements Théoriques

### Les Trois Règles de Reynolds

#### 1. Séparation (Separation)
Évite les collisions en s'éloignant des voisins trop proches.

#### 2. Alignement (Alignment)
Adopte la direction moyenne des voisins pour un mouvement coordonné.

#### 3. Cohésion (Cohesion)
Se dirige vers le centre de masse du groupe de voisins.

### Field of View

Limite la perception aux boids visibles, excluant ceux derrière :

## 🚀 Architecture du Système

### Classes Principales

#### `ABoids`
Entité boid individuelle avec tous les comportements :
- Gestion des 3 règles de Reynolds
- Évitement d'obstacles par raycasting
- Intégration avec la grille spatiale
- Paramètres configurables en temps réel

#### `ABoidsManager`
Gestionnaire central du système :
- Spawning de boids avec distribution aléatoire
- Configuration globale des paramètres
- Intégration avec le système de grille spatiale

#### `ASpatialGridManager`
Système d'optimisation avancé :
- Partitionnement de l'espace en cellules
- Recherche de voisins en O(1) par boid
- Mise à jour dynamique des positions
- Singleton pattern pour accès global

### Optimisation Spatiale

La grille spatiale divise l'espace en cellules, permettant de :
- **Réduire la complexité** de O(n²) à O(n)
- **Limiter les calculs** aux voisins proches
- **Supporter des milliers de boids** simultanément
- **Contributeurs open source** pour les améliorations continues

---

**Créez des simulations époustouflantes avec la puissance des boids et d'Unreal Engine** 🐦✨
