import pandas as pd
import optuna
import numpy as np
from sklearn.ensemble import (
    RandomForestClassifier,
    BaggingClassifier,
    VotingClassifier,
    StackingClassifier,
    ExtraTreesClassifier,
    GradientBoostingClassifier,
)
from sklearn.svm import SVC
from sklearn.linear_model import LogisticRegression, RidgeClassifier
from sklearn.naive_bayes import MultinomialNB
from sklearn.tree import DecisionTreeClassifier
from sklearn.neural_network import MLPClassifier
from xgboost import XGBClassifier
from lightgbm import LGBMClassifier
from catboost import CatBoostClassifier

df = pd.read_csv("/kaggle/input/bdaio-2024-the-clothing-sale/mnist_train.csv")
dt = pd.read_csv("/kaggle/input/bdaio-2024-the-clothing-sale/mnist_test.csv")
X = df.drop(columns=["ID", "label"])
Y = df["label"]
test = dt.drop(columns=["ID"], errors="ignore")


def func(trial, model):
    if model == "RandomForest":
        n_estimators = trial.suggest_int("n_estimators", 50, 500)
        max_depth = trial.suggest_int("max_depth", 2, 20)
        min_samples_split = trial.suggest_int("min_samples_split", 2, 20)
        fm = RandomForestClassifier(
            n_estimators=n_estimators,
            max_depth=max_depth,
            min_samples_split=min_samples_split,
            random_state=42,
        )
    elif model == "SVC":
        C = trial.suggest_float("C", 0.1, 10.0, log=True)
        gamma = trial.suggest_float("gamma", 0.001, 1.0, log=True)
        fm = SVC(C=C, gamma=gamma, probability=True, random_state=42)
    elif model == "DecisionTree":
        max_depth = trial.suggest_int("max_depth", 2, 20)
        fm = DecisionTreeClassifier(max_depth=max_depth, random_state=42)
    elif model == "MLP":
        hidden_layer_sizes = trial.suggest_categorical(
            "hidden_layer_sizes", [(50,), (100,), (50, 50)]
        )
        alpha = trial.suggest_float("alpha", 1e-5, 1e-1, log=True)
        learning_rate_init = trial.suggest_float(
            "learning_rate_init", 1e-4, 1e-1, log=True
        )
        fm = MLPClassifier(
            hidden_layer_sizes=hidden_layer_sizes,
            alpha=alpha,
            learning_rate_init=learning_rate_init,
            max_iter=500,
            random_state=42,
        )
    elif model == "XGB":
        n_estimators = trial.suggest_int("n_estimators", 50, 500)
        max_depth = trial.suggest_int("max_depth", 2, 20)
        learning_rate = trial.suggest_float("learning_rate", 0.01, 0.3, log=True)
        fm = XGBClassifier(
            n_estimators=n_estimators,
            max_depth=max_depth,
            learning_rate=learning_rate,
            use_label_encoder=False,
            eval_metric="mlogloss",
            random_state=42,
        )
    elif model == "Cat":
        iterations = trial.suggest_int("iterations", 50, 500)
        depth = trial.suggest_int("depth", 2, 20)
        learning_rate = trial.suggest_float("learning_rate", 0.01, 0.3, log=True)
        fm = CatBoostClassifier(
            iterations=iterations,
            depth=depth,
            learning_rate=learning_rate,
            verbose=0,
            random_state=42,
        )
    elif model == "LGBM":
        n_estimators = trial.suggest_int("n_estimators", 50, 500)
        max_depth = trial.suggest_int("max_depth", 2, 20)
        learning_rate = trial.suggest_float("learning_rate", 0.01, 0.3, log=True)
        fm = LGBMClassifier(
            n_estimators=n_estimators,
            max_depth=max_depth,
            learning_rate=learning_rate,
            random_state=42,
        )
    elif model == "ExtraTrees":
        n_estimators = trial.suggest_int("n_estimators", 50, 500)
        max_depth = trial.suggest_int("max_depth", 2, 20)
        min_samples_split = trial.suggest_int("min_samples_split", 2, 20)
        fm = ExtraTreesClassifier(
            n_estimators=n_estimators,
            max_depth=max_depth,
            min_samples_split=min_samples_split,
            random_state=42,
        )
    elif model == "GradientBoosting":
        n_estimators = trial.suggest_int("n_estimators", 50, 500)
        learning_rate = trial.suggest_float("learning_rate", 0.01, 0.3, log=True)
        max_depth = trial.suggest_int("max_depth", 2, 10)
        fm = GradientBoostingClassifier(
            n_estimators=n_estimators,
            learning_rate=learning_rate,
            max_depth=max_depth,
            random_state=42,
        )
    elif model == "Ridge":
        alpha = trial.suggest_float("alpha", 1e-3, 10.0, log=True)
        fm = RidgeClassifier(alpha=alpha)
    else:
        raise ValueError(f"Unknown Model: {model}")

    from sklearn.model_selection import cross_val_score

    score = cross_val_score(fm, X, Y, cv=3, scoring="accuracy").mean()
    return score


models = [
    "RandomForest",
    "SVC",
    "DecisionTree",
    "MLP",
    "XGB",
    "Cat",
    "LGBM",
    "ExtraTrees",
    "GradientBoosting",
    "Ridge",
]
bestmodels = {}

for name in models:
    study = optuna.create_study(direction="maximize")
    study.optimize(lambda trial: func(trial, name), n_trials=20)
    bestmodels[name] = study.best_params

bestrf = RandomForestClassifier(**bestmodels["RandomForest"], random_state=42)
bestsvc = SVC(**bestmodels["SVC"], probability=True, random_state=42)
bestdecisiontree = DecisionTreeClassifier(**bestmodels["DecisionTree"], random_state=42)
bestmlp = MLPClassifier(**bestmodels["MLP"], max_iter=500, random_state=42)
bestxgb = XGBClassifier(
    **bestmodels["XGB"],
    use_label_encoder=False,
    eval_metric="mlogloss",
    random_state=42,
)
bestcat = CatBoostClassifier(**bestmodels["Cat"], verbose=0, random_state=42)
bestlgbm = LGBMClassifier(**bestmodels["LGBM"], random_state=42)
bestgradient = GradientBoostingClassifier(
    **bestmodels["GradientBoosting"], random_state=42
)
bestextratrees = ExtraTreesClassifier(**bestmodels["ExtraTrees"], random_state=42)
bestridge = RidgeClassifier(**bestmodels["Ridge"])

stack1 = StackingClassifier(
    estimators=[("rf", bestrf), ("xgb", bestxgb), ("ridge", bestridge)],
    final_estimator=LogisticRegression(),
    cv=5,
)
stack2 = StackingClassifier(
    estimators=[("et", bestextratrees), ("lgbm", bestlgbm), ("svc", bestsvc)],
    final_estimator=LogisticRegression(),
    cv=5,
)
stack3 = StackingClassifier(
    estimators=[("cat", bestcat), ("gb", bestgradient), ("mlp", bestmlp)],
    final_estimator=LogisticRegression(),
    cv=5,
)
stack4 = StackingClassifier(
    estimators=[
        ("bag", BaggingClassifier()),
        ("dt", bestdecisiontree),
        ("mnb", MultinomialNB()),
    ],
    final_estimator=LogisticRegression(),
    cv=5,
)

finalensemble = VotingClassifier(
    estimators=[
        ("stack1", stack1),
        ("stack2", stack2),
        ("stack3", stack3),
        ("stack4", stack4),
    ],
    voting="soft",
)

finalensemble.fit(X, Y)
preds = finalensemble.predict(test)

submission = pd.DataFrame(
    {"ID": dt["ID"] if "ID" in dt.columns else np.arange(len(test)), "label": preds}
)
submission.to_csv("submission.csv", index=False)
