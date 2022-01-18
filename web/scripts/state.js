import {LoginView} from "./login.js";
import {MainView} from "./emails.view.js";
import {ComposeView} from "./compose.js";
import {UsersView} from "./users.js";
import {SettingsView} from "./settings.js";

export const Views = {
    Login: "view_login",
    Main: "view_main",
    Compose: "view_compose",
    Users: "view_users",
    Settings: "view_settings",
}

export let state = {
    view: Views.Login,
    loaded_views: {},
    view_classes: {
        [Views.Login]: new LoginView(),
        [Views.Main]: new MainView(),
        [Views.Compose]: new ComposeView(),
        [Views.Users]: new UsersView(),
        [Views.Settings]: new SettingsView(),
    },
    login: {
        username: "",
        token: "",
    },
    emails: [],
    selected_email: "",
    users: [],
    rules: [],
    labels: [],
}

class Controller {
    constructor() {
        this.rules = new RulesInterface()
        this.labels = new LabelsInterface()
    }
    set_view(name) {
        state.view = name;
        let vclass = state.view_classes[name]
        if (vclass) {
            if (typeof vclass.init === "function") {
                vclass.init()
            }
        }
        if (name === Views.Login) {
            let controls = document.getElementsByClassName("controls")[0];
            controls.style.display = "none";
            // history.pushState({}, null, '')
        } else {
            let controls = document.getElementsByClassName("controls")[0];
            controls.style.display = "";
            // history.pushState({}, null, '/p/' + name.split('_')[1])
        }
    }
    set(element) {
        let container = document.getElementById("container");
        container.innerHTML = "";
        container.appendChild(element);
    }
    async login(username, password) {
        let response = await fetch(`https://mail.drade.io/authorize`, {
            method: 'POST',
            body: JSON.stringify({
                username,
                password
            })
        })
        if (response.ok) {
            let data = await response.json();
            state.login.username = username;
            state.login.token = data['access_token'];
            this.save_session();
            return true
        }
        return false
    }
    async get_emails() {
        let response = await fetch('https://mail.drade.io/emails', {
            headers: {
                Authorization: `Bearer ${state.login.token}`
            }
        })
        if (response.ok) {
            state.emails = await response.json();
            return true
        } else {
            state.emails = []
        }
        return false
    }
    async get_email(name) {
        let response = await fetch(`https://mail.drade.io/emails/${name}?accept=text_html`, {
            headers: {
                Authorization: `Bearer ${state.login.token}`
            }
        })
        if (response.ok) {
            state.selected_email = await response.text();
            return true
        }
        return false
    }
    async delete_email(name) {
        let response = await fetch(`https://mail.drade.io/emails/${name}`, {
            method: 'DELETE',
            headers: {
                Authorization: `Bearer ${state.login.token}`
            }
        })
        return response.ok
    }
    async send_email(recipients, subject, body) {
        let response = await fetch("https://mail.drade.io/emails", {
            method: 'POST',
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
            body: JSON.stringify({
                from: `${state.login.username}@drade.io`,
                to: [recipients],
                subject,
                body
            })
        })
        return response.ok
    }
    async add_labels(name, labels) {
        let response = await fetch(`https://mail.drade.io/emails/${name}/labels`, {
            method: 'POST',
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
            body: JSON.stringify(labels)
        })
        return response.ok
    }
    async get_users() {
        let response = await fetch("https://mail.drade.io/users", {
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
        })
        if (response.ok) {
            state.users = await response.json()
            return true
        }
        return false
    }
    async create_user(body) {
        let response = await fetch(`https://mail.drade.io/users`, {
            method: 'POST',
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
            body: JSON.stringify(body)
        })
        return response.ok
    }
    async update_user(id, body) {
        let response = await fetch(`https://mail.drade.io/users/${id}`, {
            method: 'PUT',
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
            body: JSON.stringify(body)
        })
        return response.ok
    }
    async delete_user(id) {
        let response = await fetch(`https://mail.drade.io/users/${id}`, {
            method: 'DELETE',
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
        })
        return response.ok
    }
    notify(text) {
        let notification = document.getElementById("notification")
        notification.innerText = text;
        notification.animate([
            {opacity: 0},
            {opacity: 100},
        ], {
            duration: 250,
            fill: 'both'
        })
        setTimeout(() => {
            notification.animate([
                {opacity: 100},
                {opacity: 0},
            ], {
                duration: 250,
                fill: 'both'
            })
        }, 4000)
    }
    logout() {
        this.clear_session()
        this.set_view(Views.Login)
    }
    save_session() {
        localStorage.setItem('login.username', state.login.username);
        localStorage.setItem('login.token', state.login.token);
    }
    load_session() {
        state.login.username = localStorage.getItem("login.username") || "";
        state.login.token = localStorage.getItem("login.token") || "";
    }
    clear_session() {
        localStorage.removeItem("login.username")
        localStorage.removeItem("login.token")
    }
}

class RulesInterface {
    async get() {
        let response = await fetch("https://mail.drade.io/rules", {
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
        })
        if (response.ok) {
            await response.json().then(data => {
                state.rules = data
            }).catch(() => {
                state.rules = []
            })
            return true
        }
        return false
    }
    async create(body) {
        let response = await fetch("https://mail.drade.io/rules", {
            method: "POST",
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
            body: JSON.stringify(body)
        })
        return response.ok
    }
    async delete(id) {
        let response = await fetch(`https://mail.drade.io/rules/${id}`, {
            method: "DELETE",
            headers: {
                Authorization: `Bearer ${state.login.token}`
            }
        })
        return response.ok
    }
}


class LabelsInterface {
    async get() {
        let response = await fetch("https://mail.drade.io/labels", {
            headers: {
                Authorization: `Bearer ${state.login.token}`
            },
        })
        if (response.ok) {
            state.labels = await response.json()
            return true
        }
        return false
    }
}

export const controller = new Controller()