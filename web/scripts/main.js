import {state, Views, controller} from "./state.js";

window.addEventListener('load', async () => {
    await load_view(Views.Login);
    await load_view(Views.Main);
    await load_view(Views.Compose);
    await load_view(Views.Users);
    await load_view(Views.Settings);

    controller.load_session()

    if (state.login.token.length === 0) {
        controller.set_view(Views.Login)
    } else {
        controller.set_view(Views.Main)
        await controller.labels.get()
    }

    let logout = document.getElementById("logout_button")
    if (logout) {
        logout.onclick = () => {
            controller.logout()
        }
    }
    let users = document.getElementById("users_button")
    if (users) {
        users.onclick = () => {
            controller.set_view(Views.Users);
        }
    }
    let email = document.getElementById("email_button")
    if (email) {
        email.onclick = () => {
            controller.set_view(Views.Main);
        }
    }

    let settings = document.getElementById("settings_button")
    if (settings) {
        settings.onclick = () => {
            controller.set_view(Views.Settings);
        }
    }
});

async function load_view(name) {
    let file = name.split('_')[1];
    let response = await fetch(`/html/${file}.html`);
    if (response.ok) {
        state.loaded_views[name] = await response.text();
    }
}
